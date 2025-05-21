#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/client_session.hpp"
#include "irsol/server/image_data.hpp"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

namespace irsol {
namespace server {
namespace internal {

using CollectedFrameCallback = std::function<void(ImageData)>;

/**
 * @class FrameCollector
 * @brief Manages the continuous capture and distribution of image frames from a camera device.
 *
 * The FrameCollector operates using two dedicated threads:
 * - A **frame collection thread** that captures images from the associated Interface
 *   at a dynamically adjustable frame rate, determined by connected clients' demands.
 * - A **frame broadcast thread** that distributes captured frames to all registered clients,
 *   ensuring that each client receives frames according to their individual requested frame rates.
 *
 * Thread synchronization is handled carefully using multiple mutexes and condition variables to:
 * - Coordinate frame capture timing and allow dynamic adjustment of frame rate.
 * - Manage concurrent access to shared resources such as the client list, frame queue,
 *   and frame rate state.
 * - Signal threads to start, stop, or wait efficiently without busy-waiting.
 *
 * This design ensures thread-safe operation and efficient CPU usage by avoiding unnecessary
 * polling. The class supports dynamic client addition and removal, automatically adjusting frame
 * capture rates based on current client demands.
 *
 * Usage:
 * - Constructed with a reference to a Interface.
 * - Clients can be added or removed at runtime via thread-safe methods.
 * - The collector can be stopped cleanly, joining internal threads and releasing resources.
 *
 * Thread Safety:
 * - Public methods are safe to call concurrently from multiple threads.
 * - Internal synchronization uses separate mutexes for clients, frame queue, and frame rate state.
 * - Condition variables avoid busy-waiting and enable prompt thread wakeups on state changes.
 */
class FrameCollector {

  static constexpr uint8_t MAX_FRAME_QUEUE_SIZE = 16;
  using clients_pool_t =
      std::vector<std::pair<std::shared_ptr<ClientSession>, CollectedFrameCallback>>;
  using frame_queue_t = std::queue<ImageData>;

public:
  FrameCollector(camera::Interface &cam);
  FrameCollector(const FrameCollector &) = delete;
  FrameCollector &operator=(const FrameCollector &) = delete;

  void refreshFrameRate();
  void addClient(std::shared_ptr<ClientSession> client, CollectedFrameCallback callback);
  void removeClient(std::shared_ptr<ClientSession> client);
  void stop();

private:
  bool hasClient(std::shared_ptr<ClientSession> client) const;

  std::atomic<bool> m_running{false};

  camera::Interface &m_cam;

  std::mutex m_frameRateMutex;
  std::atomic<double> m_frameRate{0.0};

  mutable std::mutex m_clientsMutex; // also used in const-method
  clients_pool_t m_clients;

  std::mutex m_frameQueueMutex;
  frame_queue_t m_frameQueue;

  // mutex and condition_variable for collectFrames waiting on m_frameRate changes
  std::mutex m_frameRateCondMutex;
  std::condition_variable m_frameRateCond;

  // mutex and condition_variable for broadcastFrames waiting on new frames/clients
  std::mutex m_frameAvailableCondMutex;
  std::condition_variable m_frameAvailableCond;

  std::thread m_frameCollectionThread;
  void collectFrames();

  std::thread m_broadcastThread;
  void broadcastFrames();
};
} // namespace internal
} // namespace server
} // namespace irsol