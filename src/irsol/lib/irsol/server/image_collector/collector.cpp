#include "irsol/server/image_collector/collector.hpp"

#include "irsol/macros.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>

namespace irsol {
namespace server {

namespace frame_collector {

FrameCollector::FrameCollector(irsol::camera::Interface& camera): m_cam(camera) {}

FrameCollector::~FrameCollector()
{
  stop();
}

void
FrameCollector::start()
{
  m_stop.store(false);
  m_distributorThread = std::thread(&FrameCollector::run, this);
}

void
FrameCollector::stop()
{
  m_stop.store(true);
  m_scheduleCondition.notify_all();
  if(m_distributorThread.joinable())
    m_distributorThread.join();
}

bool
FrameCollector::isBusy() const
{
  return !m_clients.empty();
}

void
FrameCollector::registerClient(
  irsol::types::client_id_t                      clientId,
  double                                         fps,
  std::shared_ptr<FrameCollector::frame_queue_t> queue,
  int64_t                                        frameCount)
{

  std::lock_guard<std::mutex> lock(m_clientsMutex);

  std::chrono::microseconds interval;
  bool                      immediate = false;
  if(frameCount == 1 && fps <= 0.0) {
    // Client is requesting a single frame immediately
    interval   = std::chrono::microseconds(1);
    immediate  = true;
    frameCount = 1;
    fps        = 0.0;
  } else {
    interval = std::chrono::microseconds(static_cast<uint64_t>(1000000.0 / fps));
  }

  // Compute the scheduled time for the new client trying to use an existing schedule.
  // Allow at max to "be away" from the desired schedule by the smallest of 500ms and
  // half of the new client's interval.
  auto maxDiffBetweenDesiredAndActualDueTime = std::min(
    interval / 2, std::chrono::duration_cast<decltype(interval)>(std::chrono::milliseconds(500)));
  const auto nextDue =
    computeNextDueTimeForNewClient(interval, maxDiffBetweenDesiredAndActualDueTime);

  IRSOL_LOG_INFO(
    "Registering client {} with frame rate {} fps (interval {}), "
    "next frame due at {} #frames {}, immediate {}",
    clientId,
    fps,
    irsol::utils::durationToString(interval),
    irsol::utils::timestampToString(nextDue),
    frameCount,
    immediate);

  m_clients.emplace(
    clientId, ClientCollectionParams(fps, interval, nextDue, queue, frameCount, immediate));
  schedule(clientId, nextDue);
}

void
FrameCollector::deregisterClient(irsol::types::client_id_t clientId)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  // Mark the client's queue as finished, so the client is notified that
  // no more data will be pushed to him
  auto& clientParams = m_clients.at(clientId);
  clientParams.queue->producerFinished();

  // Removes the client from the storage
  m_clients.erase(clientId);

  auto  dueNext      = clientParams.nextFrameDue;
  auto& clientsAtDue = m_scheduleMap.at(dueNext);

  clientsAtDue.erase(
    std::remove(clientsAtDue.begin(), clientsAtDue.end(), clientId), clientsAtDue.end());
  if(clientsAtDue.empty()) {
    m_scheduleMap.erase(dueNext);
  }

  m_scheduleCondition.notify_one();
}

void
FrameCollector::run()
{
  std::unique_lock<std::mutex> lock(m_clientsMutex);

  while(!m_stop) {
    if(m_scheduleMap.empty()) {
      m_scheduleCondition.wait(lock, [this]() { return m_stop || !m_scheduleMap.empty(); });
    }

    if(m_stop)
      return;

    // Retrieve the nextDue time from the schedule map.
    // This map is always sorted from small to high, as it's an ordered container.
    irsol::types::timepoint_t nextDue = m_scheduleMap.begin()->first;
    IRSOL_LOG_INFO("Min next due is {}", irsol::utils::timestampToString(nextDue));

    // Wait until the 'm_scheduleCondition is triggered externally, or until one of the conditions
    // defined in the wait_until body is met.
    m_scheduleCondition.wait_until(lock, nextDue, [this, nextDue]() {
      // Wait until one of the following conditions happen:
      // - either m_stop becomes 'true'
      // - or a new client is registered to the collector, which has a
      //   due time which is smaller than the current nextDue timestamp.
      return m_stop || m_scheduleMap.begin()->first < nextDue;
    });

    if(m_stop)
      return;

    // Refresh the nextDue, as the above condition might have finished due to a new client being
    // registered earlier than the 'nextDue' time that was initially selected.
    nextDue = m_scheduleMap.begin()->first;

    // Clients due now or earlier
    irsol::types::timepoint_t now = irsol::types::clock_t::now();
    IRSOL_LOG_INFO("Woken up at timestamp {}", irsol::utils::timestampToString(now));
    auto readyClients = collectReadyClients(now);

    lock.unlock();  // Allow other threads to register clients

    auto [frameMetadata, imageRawBuffer] = grabImageData();

    // Deliver the frame to clients
    lock.lock();
    std::vector<irsol::types::client_id_t> finishedClient;
    for(auto clientId : readyClients) {
      auto& clientParams = m_clients.at(clientId);
      // Push a new frame created on the fly to the current client's queue.
      // This creates a copy of the image data into the queue, and this is wanted, so that if a
      // consumer modifies the image, it doesn't affect other clients.
      clientParams.queue->push(std::make_unique<Frame>(
        frameMetadata,
        irsol::protocol::ImageBinaryData(
          {imageRawBuffer.begin(), imageRawBuffer.end()},
          {frameMetadata.height, frameMetadata.width},
          {})));

      // Try to schedule the client, if no longer needed, register it in the finishedClients
      auto maxDiffBetweenDesiredAndActualDueTime = clientParams.interval / 5;
      if(!schedule(clientId, clientParams.nextFrameDue + clientParams.interval)) {
        finishedClient.push_back(clientId);
      }
    }

    // Unlock the clients so we can remove finished clients
    lock.unlock();
    for(const auto& clientId : finishedClient) {
      IRSOL_LOG_INFO(
        "Deregistering client {}, as it has consumed all the frames it needed.", clientId);
      deregisterClient(clientId);
    }

    lock.lock();
  }
}

std::vector<irsol::types::client_id_t>
FrameCollector::collectReadyClients(irsol::types::timepoint_t now)
{
  std::vector<irsol::types::client_id_t> readyClients;

  while(!m_scheduleMap.empty() && m_scheduleMap.begin()->first <= now) {
    auto& dueClients = m_scheduleMap.begin()->second;
    readyClients.insert(readyClients.end(), dueClients.begin(), dueClients.end());

    // Also remove the current list of clients from the schedule map
    m_scheduleMap.erase(m_scheduleMap.begin());
  }

  return readyClients;
}

std::pair<FrameMetadata, std::vector<irsol::types::byte_t>>
FrameCollector::grabImageData() const
{
  // Capture the frame just-in-time
  IRSOL_MAYBE_UNUSED auto t0    = irsol::types::clock_t::now();
  auto                    image = m_cam.captureImage();
  IRSOL_MAYBE_UNUSED auto t1    = irsol::types::clock_t::now();
  IRSOL_LOG_DEBUG(
    "Capture image: start: {}, stop: {}, duration: {}",
    irsol::utils::timestampToString(t0),
    irsol::utils::timestampToString(t1),
    irsol::utils::durationToString(t1 - t0));

  // Extract the image data from the image buffer, and copy it into an
  // owning structure. In this way, when `image` is destroyed at the end of the
  // execution of this function, it can return into the pool of NeoAPI::Images
  // for next frames to be written to the buffer.
  auto* imageData = image.GetImageData();
  auto  numBytes  = image.GetSize();

  std::vector<irsol::types::byte_t> rawData(numBytes);
  std::memcpy(rawData.data(), imageData, numBytes);

  return std::make_pair<FrameMetadata, std::vector<irsol::types::byte_t>>(
    {irsol::types::clock_t::now(), image.GetImageID(), image.GetHeight(), image.GetWidth()},
    std::move(rawData));
}

irsol::types::timepoint_t
FrameCollector::computeNextDueTimeForNewClient(
  irsol::types::duration_t interval,
  irsol::types::duration_t maxDiff) const
{
  const auto now = irsol::types::clock_t::now();

  // Find a client in the existing pool (if any) that has a similar nextDue time
  if(m_clients.size() == 0) {
    // No client exists, yet, so we're free to choose a time for this client.
    IRSOL_LOG_INFO(
      "No client is yet registered, setting the next time due according to the client desire.");
    return now + interval;
  }

  const auto               desiredNextDue = now + interval;
  irsol::types::duration_t minTimeOffset  = std::chrono::hours::max();

  std::vector<irsol::types::timepoint_t> existingScheduledTimes;
  existingScheduledTimes.reserve(m_scheduleMap.size());
  IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START
  for(const auto& [timepoint, _] : m_scheduleMap) {
    IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
    existingScheduledTimes.push_back(timepoint);
  }

  const auto bestExistingNextDue = *std::min_element(
    existingScheduledTimes.cbegin(),
    existingScheduledTimes.cend(),
    [&desiredNextDue](const auto& left, const auto& right) -> bool {
      IRSOL_LOG_INFO(
        "Comparing timestamps '{}' and '{}'",
        irsol::utils::timestampToString(left),
        irsol::utils::timestampToString(right));
      auto leftDiff  = std::max(left, desiredNextDue) - std::min(left, desiredNextDue);
      auto rightDiff = std::max(right, desiredNextDue) - std::min(right, desiredNextDue);
      IRSOL_LOG_INFO("Results in left < right ? {}", (leftDiff < rightDiff ? "true" : "false"));
      return leftDiff < rightDiff;
    });

  if(
    (std::max(bestExistingNextDue, desiredNextDue) -
     std::min(bestExistingNextDue, desiredNextDue)) > maxDiff) {
    IRSOL_LOG_WARN("No good existing schedule found forcing the desired schedule");
    return desiredNextDue;
  }

  IRSOL_LOG_INFO(
    "At least one client already exists in the collector, aligning nextDue time for new client "
    "to best match of existing client(s): {}->{}",
    irsol::utils::timestampToString(desiredNextDue),
    irsol::utils::timestampToString(bestExistingNextDue));

  // We now found a time due that is appropriate for this new client
  return bestExistingNextDue;
}

bool
FrameCollector::schedule(
  const irsol::types::client_id_t clientId,
  irsol::types::timepoint_t       nextFrameDue)
{

  // Update the parameters of the client.
  auto& clientParams = m_clients.at(clientId);
  if(clientParams.remainingFrames-- > 0 && clientParams.remainingFrames == 0) {
    // Client no longer expects frames.
    // This handles also clients that are listening forever, as their 'remainingFrames' is negative
    // so this¨ condition is never fully met.
    return false;
  }

  if(clientParams.nextFrameDue == nextFrameDue) {
    // This has been called the first time for client-registration
    IRSOL_LOG_INFO(
      "Client {} has been scheduled for timestamp {}.",
      clientId,
      irsol::utils::timestampToString(clientParams.nextFrameDue));
  } else {

    IRSOL_LOG_DEBUG(
      "(Rescheduling client {} for next frame, previous due: {}, next due {}, # count {}",
      clientId,
      irsol::utils::timestampToString(clientParams.nextFrameDue),
      irsol::utils::timestampToString(nextFrameDue),
      clientParams.remainingFrames);
  }

  // Updates the parameters of the client
  clientParams.nextFrameDue = nextFrameDue;

  // Registers the client for the scheduled timestamp.
  m_scheduleMap[nextFrameDue].push_back(clientId);

  std::stringstream ss;
  for(const auto& [dueSchedule, scheduledClients] : m_scheduleMap) {
    ss << "\t* " << irsol::utils::timestampToString(dueSchedule) << " -> "
       << scheduledClients.size() << " clients\n";
  }
  IRSOL_LOG_DEBUG("Number of different schedules: {}\n{}", m_scheduleMap.size(), ss.str());

  m_scheduleCondition.notify_one();
  return true;
}

}  // namespace frame_collector
}  // namespace server
}  // namespace irsol