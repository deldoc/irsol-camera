/**
 * @file irsol/server/client/state.hpp
 * @brief Defines session-specific internal state structures for connected clients.
 * @ingroup Server
 *
 * This file contains the definitions of data structures that encapsulate all
 * private, per-client state for the server's session management. Every instance
 * of a connected client is represented by a unique @ref irsol::server::ClientSession, and all
 * client-specific state related to frame streaming, communication parameters, and threading is
 * encapsulated in this module via @ref irsol::server::internal::UserSessionData.
 *
 * Communication over the socket, client-specific control logic, and thread-based
 * streaming control are all coordinated using these internal structures.
 *
 * These structures are not intended to be accessed concurrently without external
 * synchronization (e.g., session-level mutexes). Instead, they act as a container
 * for client-local data and control flags that drive runtime behaviors like
 * image frame streaming (GIS mode) or message throttling.
 */

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
 * @ingroup Server
 * @brief Per-client configuration parameters for the GIS (GetImageSequence) command.
 *
 * These values determine how many frames are sent, how they are sequenced,
 * and at what frame rate. This structure is unique to each connected client.
 */
struct GisParams
{
  /// Number of frames the client expects in a sequence.
  uint64_t inputSequenceLength{16};

  /// Current frame index in the sequence (automatically incremented).
  uint64_t inputSequenceNumber{0};

  /// Desired frame rate for this client's stream.
  double frameRate{4.0};
};

/**
 * @ingroup Server
 * @brief Controls how a client listens for frames, including thread management and GIS parameters.
 *
 * Each client that subscribes to frame streaming operates with its own
 * `FrameListeningState` instance. This object handles:
 * - Streaming parameters (GIS),
 * - Background thread lifecycle,
 * - Cancellation flags.
 *
 * Thread safety for start/stop is ensured using an internal mutex.
 */
struct FrameListeningState
{
  /// Parameters for image streaming (GIS command).
  GisParams gisParams{};

  /**
   * @brief Returns whether a frame listening thread is currently active.
   * @return `true` if a thread is running, `false` otherwise.
   */
  bool running() const;

  /**
   * @brief Starts the passed lambda function in a background thread.
   *
   * This method creates a detached thread that executes the user-provided task.
   * The task receives a shared stop token that can be polled to determine when
   * the thread should terminate.
   *
   * @warning is responsibility of the function passed as argument to stop
   * it's execution when the stop flag is set.
   *
   * @tparam Callable A callable accepting a `std::shared_ptr<std::atomic<bool>>` as stop flag.
   * @param task The function to execute inside the background thread.
   * @param description A human-readable label for logging and error tracing.
   *
   * @throws std::logic_error if a thread is already running.
   *
   * @note Thread lifecycle is managed internally. Use `stop()` to request termination.
   * @note This feature is used by the _frame listening_ mechanism, to allow a @ref
   * irsol::server::ClientSession to listen to frames to be sent back to the client, all while
   * maintaining the capability to respond to new client's submitted messages. This is because the
   * frame-listening process is pushed into the listening background thread.
   */
  template<typename Callable>
  void start(Callable&& task, const std::string& description)
  {
    std::scoped_lock<std::mutex> lock(m_threadMutex);
    if(m_running.load()) {
      throw std::logic_error("Thread already running");
    }

    IRSOL_LOG_INFO("Locking the running of the thread");
    m_stopRequested = std::make_shared<std::atomic<bool>>(false);
    m_running.store(true);

    std::thread([this,
                 stopRequested = m_stopRequested,
                 task          = std::forward<Callable>(task),
                 description]() mutable {
      try {
        IRSOL_LOG_INFO("Inside thread");
        task(stopRequested);
        IRSOL_LOG_INFO("Inside thread, after task execution");
      } catch(...) {
        IRSOL_LOG_ERROR("Background thread for task '{}' failed running.", description);
      }
      m_running.store(false);
    })
      .detach();

    IRSOL_LOG_INFO("Thread started");
  }

  /**
   * @brief Requests the active thread (if any) to stop by setting the stop flag.
   *
   * This does not join the thread (which is detached), but signals the loop
   * to exit if the user task respects the stop token.
   */
  void stop();

private:
  /// Internal flag indicating whether the thread is running.
  std::atomic<bool> m_running{false};

  /// Synchronization primitive to guard start/stop operations.
  std::mutex m_threadMutex;

  /// Shared stop token used to cancel the thread's task loop.
  std::shared_ptr<std::atomic<bool>> m_stopRequested;
};

/**
 * @ingroup Server
 * @brief Encapsulates all per-client data used during a session's lifetime.
 *
 * This struct holds all session-specific state for a connected client, including:
 * - Streaming control flags and parameters
 * - Thread state for frame delivery
 *
 * It is owned and accessed by the @ref irsol::server::ClientSession class and should not be shared
 * across sessions. All socket communication and dynamic client behavior is
 * coordinated via this structure.
 */
struct UserSessionData
{
  /// Client-specific state for image frame listening, including parameters and worker thread.
  FrameListeningState frameListeningState{};
};

}  // namespace internal
}  // namespace server
}  // namespace irsol