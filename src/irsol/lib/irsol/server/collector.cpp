#include "irsol/server/collector.hpp"
#include "irsol/logging.hpp"
#include "irsol/server/app.hpp"
#include "irsol/utils.hpp"

namespace irsol {
namespace internal {

FrameCollector::FrameCollector(CameraInterface &cam) : m_cam(cam) {
  m_running = true;
  // Start frame collection thread
  m_frameCollectionThread = std::thread([this]() { collectFrames(); });
  // Start frame distribution thread
  m_broadcastThread = std::thread([this]() { broadcastFrames(); });
}

void FrameCollector::refreshFrameRate() {
  std::lock_guard<std::mutex> clientsLock(m_clientsMutex);

  if (m_clients.empty()) {
    IRSOL_LOG_INFO("No clients connected to frame collector, setting frame rate to 0.");
    {
      std::lock_guard<std::mutex> frameRateLock(m_frameRateCondMutex);
      m_frameRate.store(0.0);
    }
    m_frameRateCond.notify_all();
    return;
  }

  std::vector<double> frameRates;
  frameRates.reserve(m_clients.size());
  for (auto &[client, callback] : m_clients) {
    frameRates.push_back(client->sessionData().frameListeningParams.frameRate);
  }

  double clientsMaxFrameRate = *std::max_element(frameRates.begin(), frameRates.end());

  for (double r : frameRates) {
    if (r <= 0.0) {
      continue;
    }
    double ratio = clientsMaxFrameRate / r;
    double rounded = std::round(ratio);
    const double eps = 1e-6;
    if (std::abs(ratio - rounded) > eps) {
      IRSOL_LOG_WARN("Client rate {:.2f} fps is not an integer divisor of max fps {:.2f}; "
                     "that client's frames may never land exactly on its desired schedule",
                     r, clientsMaxFrameRate);
    }
  }

  {
    // Protect the update of the frameRate
    // as this variable is living in a multithreaded environment.
    std::lock_guard<std::mutex> frameRateLock(m_frameRateMutex);

    if (m_frameRate.load() != clientsMaxFrameRate) {

      IRSOL_LOG_INFO("Updating collector frame rate to {:.2f}", clientsMaxFrameRate);
      std::string frameRatesStr = "";
      for (auto rate : frameRates) {
        frameRatesStr += std::to_string(rate) + ", ";
      }
      IRSOL_LOG_DEBUG("Frame rates: {}", frameRatesStr);

      m_frameRate.store(clientsMaxFrameRate);

      m_frameRateCond.notify_all();
    }
  }
}

bool FrameCollector::hasClient(std::shared_ptr<ClientSession> client) const {
  std::lock_guard<std::mutex> lock(m_clientsMutex);
  return std::any_of(m_clients.begin(), m_clients.end(),
                     [client](const auto &pair) { return pair.first == client; });
}

void FrameCollector::addClient(std::shared_ptr<ClientSession> client,
                               CollectedFrameCallback callback) {

  if (hasClient(client)) {
    IRSOL_LOG_WARN(
        "Client already exists in frame collector, removing old one and replacing with new one.");
    removeClient(client);
  }
  {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    m_clients.emplace_back(client, callback);
    IRSOL_LOG_INFO("Client {} added to frame collector, total clients: {}", client->id(),
                   m_clients.size());
  }
  m_frameAvailableCond.notify_all();
  refreshFrameRate();
}

void FrameCollector::removeClient(std::shared_ptr<ClientSession> client) {
  {
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    auto it = std::find_if(m_clients.begin(), m_clients.end(),
                           [client](const auto &pair) { return pair.first == client; });
    if (it != m_clients.end()) {
      m_clients.erase(it);
      IRSOL_LOG_INFO("Client {} removed from frame collector, total clients: {}", client->id(),
                     m_clients.size());
      m_frameAvailableCond.notify_all();
    } else {
      IRSOL_LOG_WARN("Client not found in frame collector");
    }
  }
  refreshFrameRate();
}

void FrameCollector::stop() {
  IRSOL_LOG_DEBUG("Stopping frame collector");
  m_running.store(false);

  IRSOL_LOG_DEBUG("Stopping frame collection thread");
  m_frameRateCond.notify_all(); // Wake up collectFrames if it's waiting
  m_frameCollectionThread.join();

  IRSOL_LOG_DEBUG("Stopping frame broadcast thread");
  m_frameAvailableCond.notify_all(); // Wake up broadcastFrames if it's waiting
  m_broadcastThread.join();

  IRSOL_LOG_DEBUG("Frame collector stopped");
}

void FrameCollector::collectFrames() {

  auto lastFrameTime = std::chrono::steady_clock::now();
  std::unique_lock<std::mutex> frameRateCondLock(m_frameRateCondMutex);

  while (true) {
    // atomically unlocks the mutex and suspends the thread
    // until the condition variable is notified and the predicate becomes true.
    // Once the predicate becomes true, the mutex is locked again.
    m_frameRateCond.wait(frameRateCondLock, [this]() {
      if (!m_running.load()) {
        // running abort request, exit the condition
        // and allow outer loop to break/exit.
        IRSOL_LOG_DEBUG("Frame collection stop request from inside thread received");
        return true;
      }

      const double currentFrameRate = m_frameRate.load();
      if (currentFrameRate <= 0.0) {
        // A negative frame rate is still set,
        // wait until a positive frame rate is set.
        IRSOL_LOG_INFO("Frame collection paused, as no positive frame rate is set.");
        return false;
      }
      // A positive frame rate is available, exit the condition
      // and allow the collection to start.
      return true;
    });

    // we woke up: figure out why
    if (!m_running.load()) {
      IRSOL_LOG_DEBUG("Frame collection stop request from inside thread received");
      break;
    }

    const double currentFrameRate = m_frameRate.load();
    const auto timeBetweenFrames =
        std::chrono::microseconds(static_cast<uint64_t>(1000000 / currentFrameRate));

    // Calculate next frame capture time
    auto nextFrameTime = lastFrameTime + timeBetweenFrames;

    // Here we unlock condLock before sleeping because:
    // - We want to allow other threads to notify the condition variable and/or update m_frameRate
    // while this thread is sleeping.
    // - Keeping frameRateCondLock locked during sleep_until() would block those other threads from
    // acquiring the mutex and modifying m_frameRate or notifying the condition variable.
    // - Unlocking is critical to avoid deadlocks or delays
    IRSOL_LOG_TRACE("Sleeping until time for capturing next frame");
    frameRateCondLock.unlock();
    std::this_thread::sleep_until(nextFrameTime);
    IRSOL_LOG_TRACE("Waking up from sleep");

    auto image = m_cam.captureImage();
    IRSOL_LOG_DEBUG("Captured frame at time {}", utils::timestamp_to_str(nextFrameTime));

    // Extract the image data into a structure suitable for transmission
    size_t width = image.GetWidth();
    size_t height = image.GetHeight();
    size_t dataSize = image.GetSize();
    size_t imageId = image.GetImageID();

    const void *imageBuffer = image.GetImageData();
    void *rawBuffer = new char[dataSize];
    memcpy(rawBuffer, imageBuffer, dataSize);

    std::shared_ptr<void> buffer(rawBuffer, [](void *p) { delete[] static_cast<char *>(p); });

    {
      std::lock_guard<std::mutex> lock(m_frameQueueMutex);
      ImageData imageData{buffer, dataSize, nextFrameTime, height,
                          width,  1,        imageId}; // Assuming 1 channel for now (RGB)
      m_frameQueue.push(std::move(imageData));
    }

    // Update the time of the last frame capture for the next iteration.
    lastFrameTime = nextFrameTime;

    m_frameAvailableCond.notify_all();

    // Lock the condition's variable lock for the next frame
    frameRateCondLock.lock();
  }
}

void FrameCollector::broadcastFrames() {

  std::unique_lock<std::mutex> frameAvailableCondLock(m_frameAvailableCondMutex);

  while (true) {
    // atomically unlocks the mutex and suspends the thread
    // until the condition variable is notified and the predicate becomes true.
    // Once the predicate becomes true, the mutex is locked again.
    m_frameAvailableCond.wait(frameAvailableCondLock, [this] {
      if (!m_running.load()) {
        // In this case, we release the condition variable's lock
        // as we want the thread to exit.
        IRSOL_LOG_DEBUG("Frame broadcast stop request from inside thread received");
        return true;
      }
      const size_t numClients = m_clients.size();
      const size_t numFrames = m_frameQueue.size();
      if (numClients == 0 || numFrames == 0) {
        // In this case, there's no clients, nor frames to be broadcasted.
        // So we keep the condition variable's lock until there's something to do.
        IRSOL_LOG_TRACE(
            "Frame broadcast paused, as no clients ({}) or no frames ({}) are available.",
            numClients, numFrames);
        return false;
      }

      // There's work to do, so release the condition variable.
      IRSOL_LOG_TRACE("Frame broadcaster thread is awakened for {} clients, and {} frames.",
                      numClients, numFrames);
      return true;
    });

    if (!m_running.load())
      break;

    ImageData imageData;
    {
      std::lock_guard<std::mutex> frameLock(m_frameQueueMutex);
      imageData = m_frameQueue.front();
      m_frameQueue.pop();
    }

    {
      std::lock_guard<std::mutex> clientLock(m_clientsMutex);
      for (auto &[client, callback] : m_clients) {
        auto &params = client->sessionData().frameListeningParams;

        // Compute when this client should have gotten its next frame
        auto interval =
            std::chrono::microseconds(static_cast<uint64_t>(1000000 / params.frameRate));
        auto clientDesiredNextFrameTime = params.lastFrameSent + interval;

        // Only send if we've reached (or passed) that scheduled point
        if (imageData.timestamp < clientDesiredNextFrameTime)
          continue;

        callback(imageData);

        // Re-anchor to the scheduled time, so we stay on a clean grid
        params.lastFrameSent = clientDesiredNextFrameTime;
      }
    }

    {
      std::lock_guard<std::mutex> frameLock(m_frameQueueMutex);
      while (m_frameQueue.size() > MAX_FRAME_QUEUE_SIZE) {
        IRSOL_LOG_WARN("Frame queue overflow, dropping frames");
        m_frameQueue.pop();
      }
    }
  }
}
} // namespace internal
} // namespace irsol