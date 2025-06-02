#pragma once

#include "irsol/logging.hpp"
#include "irsol/types.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace irsol {
namespace server {
namespace internal {

/**
 * @brief Per client parameters for Gis command
 */
struct GisParams
{
  uint64_t inputSequenceLength{16};
  uint64_t inputSequenceNumber{0};
  double   frameRate{4.0};
};

/**
 * @brief State associated with the Session client and how it's listening to frames.
 *
 */
struct FrameListeningState
{
  GisParams gisParams{};

  /// Whether the thread is running or not
  bool running() const;

  /**
   * @brief Starts the passed lambda function in a background thread.
   * @param task Lambda function to execute within the thread.
   * @note The 'm_running' state is automatically updated by this method. In order to cancel the
   * background task, use the 'stop' method.
   * */
  template<typename Callable>
  void start(Callable&& task, const std::string& description)
  {
    std::scoped_lock<std::mutex> lock(m_threadMutex);
    if(m_running.load()) {
      throw std::logic_error("Thread already running");
    }

    IRSOL_LOG_INFO("Locking the running of the thread");
    // Reset stop flag and mark as running
    m_stopRequested = std::make_shared<std::atomic<bool>>(false);
    m_running.store(true);

    std::thread([this,
                 stopRequested = m_stopRequested,
                 task          = std::forward<Callable>(task),
                 description]() mutable {
      try {
        IRSOL_LOG_INFO("INside thread");
        task(stopRequested);  // Run the actual user-provided task by passing in the stop-request
        IRSOL_LOG_INFO("Inside thread, after task execution");
      } catch(...) {
        IRSOL_LOG_ERROR("Background thread for task '{}' failed running.", description);
      }
      m_running.store(false);
    })
      .detach();

    IRSOL_LOG_INFO("Thread started");
  }
  /// Stops the background thread by setting the stop flag.
  void stop();

private:
  std::atomic<bool> m_running{false};
  /// Mutex to synchronize thread lifecycle operations
  std::mutex m_threadMutex;
  /// Flag passed into the thread for allowing main loop to stop the running thread
  std::shared_ptr<std::atomic<bool>> m_stopRequested;
};

/**
 * @brief Encapsulates all per-client data for managing a user session.
 *
 * A UserSessionData object holds the networking socket for communication,
 * synchronization primitives protecting concurrent access to the socket,
 * and frame delivery parameters that control how images are streamed to the client.
 */
struct UserSessionData
{

  /// Controls the client's state w.r.t listening to frames.
  FrameListeningState frameListeningState{};
};

}  // namespace internal
}  // namespace server
}  // namespace irsol