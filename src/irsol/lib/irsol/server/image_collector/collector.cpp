#include "irsol/server/image_collector/collector.hpp"

#include "irsol/macros.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>

namespace irsol {
namespace server {

namespace frame_collector {

std::shared_ptr<FrameCollector::frame_queue_t>
FrameCollector::makeQueuePtr()
{
  return std::make_shared<FrameCollector::frame_queue_t>();
}

FrameCollector::FrameCollector(irsol::camera::Interface& camera): m_cam(camera)
{
  start();
}

FrameCollector::~FrameCollector()
{
  stop();
}

void
FrameCollector::start()
{
  if(m_stop.load()) {
    IRSOL_NAMED_LOG_ERROR(
      "frame_collector", "Collector is already running, ignoring duplicate start request");
    return;
  }
  m_stop.store(false);
  m_distributorThread = std::thread(&FrameCollector::run, this);
}

void
FrameCollector::stop()
{
  if(!m_stop.load()) {
    IRSOL_NAMED_LOG_ERROR(
      "frame_collector", "Collector was not running, ignoring duplicate stop request");
    return;
  }
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

  std::scoped_lock<std::mutex> lock(m_clientsMutex);

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
    IRSOL_NAMED_LOG_INFO(
      "frame_collector",
      "registering client with interval of {}",
      irsol::utils::durationToString(interval));
  }

  // Registers the client so that the next due time is in SLACK ms.
  // This is to allow the collector thread to batch multiple clients, that desire a frame at a
  // specific timestamp, but all within the SLACK duration and to serve them all with the same frame
  // image.
  auto nextDue = irsol::types::clock_t::now() + FrameCollector::SLACK;

  IRSOL_NAMED_LOG_INFO(
    "frame_collector",
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
  std::scoped_lock<std::mutex> lock(m_clientsMutex);
  deregisterClientNonThreadSafe(clientId);
}

void
FrameCollector::run()
{
  std::unique_lock<std::mutex> lock(m_clientsMutex);

  while(!m_stop) {
    if(m_scheduleMap.empty()) {
      // Wait until at least one new client is registered in the map, or if a stop request has
      // arrived.
      m_scheduleCondition.wait(lock, [this]() {
        IRSOL_NAMED_LOG_DEBUG(
          "frame_collector",
          "Waiting until a client is scheduled (clients size: {}, schedule size: {})",
          m_clients.size(),
          m_scheduleMap.size());
        return m_stop || !m_scheduleMap.empty();
      });
    }

    if(m_stop.load()) {
      IRSOL_NAMED_LOG_INFO("frame_collector", "Requested frame-collection stop");
      break;
    }

    // Retrieve the nextDue time from the schedule map.
    // This map is always sorted from small to high, as it's an ordered container.
    irsol::types::timepoint_t nextDue = m_scheduleMap.begin()->first;
    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector",
      "Running for frame collection due at {}",
      irsol::utils::timestampToString(nextDue));

    // Wait at most until the `nextDue` time.
    // Allow for early break in case of:
    // - m_stop is set to true due to a stop-request
    // - a new client is registered with a nextDue time that is smaller than the current nextDue
    // time
    m_scheduleCondition.wait_until(lock, nextDue, [this, currentNextDue = nextDue]() {
      if(m_stop.load()) {
        // stopped externally, exit the wait
        return true;
      }
      if(m_scheduleMap.empty()) {
        // Don't wake up early unnecessarily, as there's no schedules in the map
        return false;
      }

      // Check if a new schedule has been inserted into the map, which happens
      // earlier than the due time we captured prior to sleeping.
      auto newNextDue = m_scheduleMap.begin()->first;

      IRSOL_NAMED_LOG_DEBUG(
        "frame_collector",
        "Current first timestamp in the map: {}, current nextDue: {}",
        irsol::utils::timestampToString(newNextDue),
        irsol::utils::timestampToString(currentNextDue));
      return newNextDue < currentNextDue;
    });

    if(m_stop.load()) {
      IRSOL_NAMED_LOG_INFO(
        "frame_collector", "Frame collection stop request received, breaking loop");
      return;
    }

    // Refresh the nextDue, as the above condition might have finished due to a new client being
    // registered earlier than the 'nextDue' time that was initially selected.
    nextDue = m_scheduleMap.begin()->first;

    // Clients due now or earlier
    irsol::types::timepoint_t now = irsol::types::clock_t::now();
    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector",
      "Woken up at timestamp {}, with next due at {}",
      irsol::utils::timestampToString(now),
      irsol::utils::timestampToString(nextDue));

    const auto slack = m_clients.size() == 1 ? std::chrono::milliseconds(0) : FrameCollector::SLACK;
    auto [readyClients, clientsSchedules] = collectReadyClients(now, slack);
    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector", "Found {} clients that need an image now!", readyClients.size());
    {
      auto uniqueSchedules =
        std::set<irsol::types::timepoint_t>(clientsSchedules.begin(), clientsSchedules.end());
      for(auto uniqueSchedule : uniqueSchedules) {
        auto numClientsWithSchedule = std::count_if(
          clientsSchedules.begin(),
          clientsSchedules.end(),
          [&uniqueSchedule](const auto& schedule) { return schedule == uniqueSchedule; });

        if(uniqueSchedule != nextDue) {
          IRSOL_NAMED_LOG_WARN(
            "frame_collector_slack",
            "Actual schedule: {}, also considering {} clients with schedule {} due to allowed "
            "slack of {}",
            irsol::utils::timestampToString(nextDue),
            numClientsWithSchedule,
            irsol::utils::timestampToString(uniqueSchedule),
            irsol::utils::durationToString(slack));
        }
      }
    }
    cleanUpSchedule(clientsSchedules);
    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector",
      "After schedule cleanup, there are still {} schedules",
      m_scheduleMap.size());

    auto grabResult = grabImageData();
    if(!grabResult) {
      IRSOL_NAMED_LOG_WARN("frame_collector", "Image acquisition failed.");
      continue;
    }
    auto& [frameMetadata, imageRawBuffer] = *grabResult;

    // Deliver the frame to clients
    std::vector<irsol::types::client_id_t> finishedClient;
    for(auto clientId : readyClients) {
      IRSOL_NAMED_LOG_DEBUG("frame_collector", "Notifying client {} for new image data", clientId);
      auto& clientParams = m_clients.at(clientId);
      // Push a new frame created on the fly to the current client's queue.
      // This creates a copy of the image data into the queue, and this is wanted, so that if a
      // consumer modifies the image, it doesn't affect other clients.
      clientParams.queue->push(std::make_unique<Frame>(
        frameMetadata,
        irsol::protocol::ImageBinaryData(
          {imageRawBuffer.begin(), imageRawBuffer.end()},
          {frameMetadata.height, frameMetadata.width},
          {irsol::protocol::BinaryDataAttribute("imageId", static_cast<int>(frameMetadata.frameId)),
           irsol::protocol::BinaryDataAttribute(
             "timestamp", irsol::utils::timestampToString(frameMetadata.timestamp))})));

      // Try to schedule the client, if no longer needed, register it in the finishedClients
      if(!schedule(clientId, clientParams.nextFrameDue + clientParams.interval)) {
        finishedClient.push_back(clientId);
      }
    }

    // Unlock the clients so we can remove finished clients
    for(const auto& clientId : finishedClient) {
      IRSOL_NAMED_LOG_DEBUG(
        "frame_collector",
        "Deregistering client {}, as it has consumed all the frames it needed.",
        clientId);
      deregisterClientNonThreadSafe(clientId);
    }

    IRSOL_NAMED_LOG_DEBUG("frame_collector", "Loop finished, restarting loop");
  }
}

void
FrameCollector::deregisterClientNonThreadSafe(irsol::types::client_id_t clientId)
{
  // Mark the client's queue as finished, so the client is notified that
  // no more data will be pushed to him
  IRSOL_NAMED_LOG_INFO("frame_collector", "Deregistering client {}", clientId);

  if(m_clients.find(clientId) == m_clients.end()) {
    IRSOL_NAMED_LOG_WARN(
      "frame_collector", "Client {} was already deregistered, ignoring request.", clientId);
    return;
  }
  auto& clientParams = m_clients.at(clientId);
  clientParams.queue->producerFinished();

  // Removes the client from the storage
  auto dueNext = clientParams.nextFrameDue;

  m_clients.erase(clientId);
  IRSOL_NAMED_LOG_DEBUG(
    "frame_collector", "Removed client {}, now remaining  {} clients", clientId, m_clients.size());

  if(m_scheduleMap.find(dueNext) == m_scheduleMap.end()) {
    IRSOL_NAMED_LOG_WARN(
      "frame_collector",
      "Scheduled time {} was already deregistered. Ignoring request.",
      irsol::utils::timestampToString(dueNext));
    return;
  }
  auto& clientsAtDue = m_scheduleMap.at(dueNext);

  IRSOL_NAMED_LOG_DEBUG(
    "frame_collector",
    "Deregistering client {} from next schedule at {}",
    clientId,
    irsol::utils::timestampToString(dueNext));
  clientsAtDue.erase(
    std::remove(clientsAtDue.begin(), clientsAtDue.end(), clientId), clientsAtDue.end());

  if(clientsAtDue.empty()) {
    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector",
      "No more clients for schedule {}. Removing schedule.",
      irsol::utils::timestampToString(dueNext));
    m_scheduleMap.erase(dueNext);
  }

  m_scheduleCondition.notify_one();
}

std::pair<std::vector<irsol::types::client_id_t>, std::vector<irsol::types::timepoint_t>>
FrameCollector::collectReadyClients(irsol::types::timepoint_t now, irsol::types::duration_t slack)
  const
{
  IRSOL_NAMED_LOG_DEBUG(
    "frame_collector",
    "Collecting clients for time {}, with slack of {}",
    irsol::utils::timestampToString(now),
    irsol::utils::durationToString(slack));
  std::vector<irsol::types::client_id_t> readyClients;
  std::vector<irsol::types::timepoint_t> clientsSchedule;

  for(const auto& [scheduleTime, clients] : m_scheduleMap) {
    // As soon as the scheduleTime of the client is above now+slack, break the loop.
    // We can break early, as m_scheduleMap is an ordered container, that is iterated over
    // in a way that smaller keys (scheduledTimes) are always iterated over at the beginning.
    if(scheduleTime > now + slack) {
      break;
    }
    for(const auto& client : clients) {
      readyClients.push_back(client);
      clientsSchedule.push_back(scheduleTime);
    }
  }

  return {readyClients, clientsSchedule};
}

void
FrameCollector::cleanUpSchedule(const std::vector<irsol::types::timepoint_t> schedules)
{
  // Erase all the schedules in the input vector
  for(auto schedule : schedules) {
    m_scheduleMap.erase(schedule);
  }
}

std::optional<std::pair<FrameMetadata, std::vector<irsol::types::byte_t>>>
FrameCollector::grabImageData() const
{
  // Capture the frame just-in-time
  IRSOL_MAYBE_UNUSED auto t0    = irsol::types::clock_t::now();
  auto                    image = m_cam.captureImage();
  IRSOL_MAYBE_UNUSED auto t1    = irsol::types::clock_t::now();
  IRSOL_NAMED_LOG_DEBUG(
    "frame_collector",
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

  if(numBytes == 0) {
    return std::nullopt;
  }

  std::vector<irsol::types::byte_t> rawData(numBytes);
  std::memcpy(rawData.data(), imageData, numBytes);

  return std::make_pair<FrameMetadata, std::vector<irsol::types::byte_t>>(
    {irsol::types::clock_t::now(), image.GetImageID(), image.GetHeight(), image.GetWidth()},
    std::move(rawData));
}

bool
FrameCollector::schedule(
  const irsol::types::client_id_t& clientId,
  irsol::types::timepoint_t        nextFrameDue)
{
  IRSOL_ASSERT_ERROR(
    m_clients.find(clientId) != m_clients.end(), "Impossible to schedule an unregistered client.");
  // Update the parameters of the client.
  auto& clientParams = m_clients.at(clientId);
  if(clientParams.remainingFrames-- == 0 && clientParams.remainingFrames < 0) {
    // Client no longer expects frames.
    // This handles also clients that are listening forever, as their 'remainingFrames' is negative
    // so this¨ condition is never fully met.
    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector", "Client {} had no longer frames to produce.", clientId);
    return false;
  }

  if(clientParams.nextFrameDue == nextFrameDue) {
    // This has been called the first time for client-registration
    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector",
      "Client {} has been scheduled for timestamp {}.",
      clientId,
      irsol::utils::timestampToString(clientParams.nextFrameDue));
  } else {

    IRSOL_NAMED_LOG_DEBUG(
      "frame_collector",
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

  // Notify the condition variable, that a new client has been scheduled
  m_scheduleCondition.notify_one();
  return true;
}

}  // namespace frame_collector
}  // namespace server
}  // namespace irsol