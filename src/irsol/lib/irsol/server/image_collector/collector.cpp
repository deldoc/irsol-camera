#include "irsol/server/image_collector/collector.hpp"

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
  m_stopFlag.store(false);
  m_distributorThread = std::thread(&FrameCollector::distributorLoop, this);
}

void
FrameCollector::stop()
{
  m_stopFlag.store(true);
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

  irsol::types::timepoint_t now     = irsol::types::clock_t::now();
  irsol::types::timepoint_t nextDue = now + interval;

  IRSOL_LOG_INFO(
    "Registering client {} ar {} with frame rate {} fps, interval {} us, nextDue {} frame count "
    "{}, "
    "immediate {}",
    clientId,
    irsol::utils::timestampToString(now),
    fps,
    interval.count(),
    irsol::utils::timestampToString(nextDue),
    frameCount,
    immediate);

  m_clients.emplace(
    clientId, ClientCollectionParams(fps, interval, nextDue, queue, frameCount, immediate));
  m_scheduleMap[nextDue].push_back(clientId);

  m_scheduleCondition.notify_one();
}

void
FrameCollector::deregisterClient(irsol::types::client_id_t clientId)
{
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  auto                        it = m_clients.find(clientId);
  if(it != m_clients.end()) {
    irsol::types::timepoint_t due = it->second.nextFrameDue;
    m_clients.erase(it);

    auto schedIt = m_scheduleMap.find(due);
    if(schedIt != m_scheduleMap.end()) {
      auto& vec = schedIt->second;
      vec.erase(std::remove(vec.begin(), vec.end(), clientId), vec.end());
      if(vec.empty()) {
        m_scheduleMap.erase(schedIt);
      }
    }

    m_scheduleCondition.notify_one();
  }
}

void
FrameCollector::distributorLoop()
{
  std::unique_lock<std::mutex> lock(m_clientsMutex);

  while(!m_stopFlag) {
    if(m_scheduleMap.empty()) {
      m_scheduleCondition.wait(lock, [this]() { return m_stopFlag || !m_scheduleMap.empty(); });
    }

    if(m_stopFlag)
      return;

    irsol::types::timepoint_t nextDue = m_scheduleMap.begin()->first;
    IRSOL_LOG_INFO("Min next due is {}", irsol::utils::timestampToString(nextDue));

    m_scheduleCondition.wait_until(lock, nextDue, [this, nextDue]() {
      return m_stopFlag || m_scheduleMap.begin()->first < nextDue;
    });

    if(m_stopFlag)
      return;

    // Clients due now or earlier
    irsol::types::timepoint_t now = irsol::types::clock_t::now();
    IRSOL_LOG_INFO("Woken up at timestamp {}", irsol::utils::timestampToString(now));
    std::vector<irsol::types::client_id_t> readyClients;

    while(!m_scheduleMap.empty() && m_scheduleMap.begin()->first <= now) {
      auto& dueClients = m_scheduleMap.begin()->second;
      readyClients.insert(readyClients.end(), dueClients.begin(), dueClients.end());
      m_scheduleMap.erase(m_scheduleMap.begin());
    }

    lock.unlock();  // Allow other threads to register clients

    // Capture the frame just-in-time
    auto t0 = irsol::types::clock_t::now();
    IRSOL_LOG_INFO(
      "Capturing image for {} clients, started at {}",
      readyClients.size(),
      irsol::utils::timestampToString(t0));
    auto image = m_cam.captureImage();
    auto t1    = irsol::types::clock_t::now();
    IRSOL_LOG_INFO(
      "Finished capturing of frame data at {}, duration: {}",
      irsol::utils::timestampToString(t1),
      irsol::utils::durationToString(t1 - t0));

    auto* imageData = image.GetImageData();
    auto  numBytes  = image.GetSize();
    auto  width     = image.GetWidth();
    auto  height    = image.GetHeight();

    std::vector<irsol::types::byte_t> rawData(numBytes);
    std::memcpy(rawData.data(), imageData, numBytes);

    FrameMetadata metadata{irsol::types::clock_t::now(), image.GetImageID()};

    // Deliver the frame to clients
    lock.lock();
    std::vector<irsol::types::client_id_t> finishedClient;
    for(auto clientId : readyClients) {
      auto it = m_clients.find(clientId);
      if(it == m_clients.end()) {
        IRSOL_LOG_WARN(
          "Client {} not found in frame collector, even if it was supposed to be due for frame "
          "distribution. Skipping",
          clientId);
        continue;
      }

      auto& clientParams = it->second;
      // Push a new frame created on the fly to the current client's queue.
      // This creates a copy of the image data into the queue, and this is wanted, so that if a
      // consumer modifies the image, it doesn't affect other clients.
      clientParams.queue->push(std::make_unique<Frame>(
        metadata,
        irsol::protocol::ImageBinaryData({rawData.begin(), rawData.end()}, {height, width}, {})));

      if(clientParams.remainingFrames > 0) {
        clientParams.remainingFrames--;
      }

      if(clientParams.remainingFrames == 0) {
        // Mark the client as finished
        finishedClient.push_back(clientId);
      } else {
        // Reschedule the client for the next frame
        auto nextDue = std::max(now, clientParams.nextFrameDue + clientParams.interval);
        IRSOL_LOG_INFO(
          "Rescheduling client {} for next frame, previous due: {}, next due {}, frame count {}",
          clientId,
          irsol::utils::timestampToString(clientParams.nextFrameDue),
          irsol::utils::timestampToString(nextDue),
          clientParams.remainingFrames);
        clientParams.nextFrameDue = nextDue;
        m_scheduleMap[nextDue].push_back(clientId);
      }
    }

    // Unlock the clients so we can remove finished clients
    lock.unlock();
    for(auto clientId : finishedClient) {
      IRSOL_LOG_INFO(
        "Deregistering client {}, as it has consumed all the frames it needed.", clientId);
      auto it = m_clients.find(clientId);
      if(it == m_clients.end()) {
        IRSOL_LOG_WARN(
          "Client {} not found in frame collector, even if it was supposed to be due for frame "
          "distribution. Skipping",
          clientId);
        continue;
      } else {
        auto& clientParams = it->second;
        clientParams.queue->producerFinished();
      }
      deregisterClient(clientId);
    }

    lock.lock();
  }
}
}  // namespace frame_collector
}  // namespace server
}  // namespace irsol