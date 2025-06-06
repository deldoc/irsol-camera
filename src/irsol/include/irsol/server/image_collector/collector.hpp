#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/server/image_collector/frame.hpp"
#include "irsol/server/image_collector/params.hpp"
#include "irsol/types.hpp"
#include "irsol/utils.hpp"

#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

/**
 * @defgroup FrameCollector FrameCollector
 * @brief Components for coordinated camera frame acquisition and distribution to clients.
 *
 * The FrameCollector module provides mechanisms to acquire frames from a camera device and
 * distribute them to multiple clients according to their requested frame rates and delivery
 * schedules. It is designed for scenarios where multiple consumers require synchronized or
 * independent access to camera data, such as in distributed vision systems, real-time monitoring,
 * or multi-client streaming applications.
 */

namespace irsol {
namespace server {
namespace frame_collector {

/**
 * @ingroup FrameCollector
 * @brief Coordinates frame acquisition from a camera and distributes frames to registered clients.
 *
 * The FrameCollector class is responsible for orchestrating the acquisition of frames from a
 * camera device and distributing those frames to multiple registered clients. Each client can
 * specify its desired frame rate and the number of frames it wishes to receive. The collector
 * ensures that frames are delivered to clients according to their schedules, batching requests
 * within a configurable "slack" window to optimize camera usage and minimize redundant captures.
 * For instance, if multiple clients are scheduled to receive frames at similar times, a single
 * frame image is requested from the camera and distributed to all those clients.
 *
 * The collector maintains a background thread that monitors client schedules, captures frames
 * just-in-time, and pushes them into client-specific @ref irsol::utils::SafeQueue. Clients can
 * consume frames from these queues at their own pace. When a client has received its requested
 * number of frames, it is automatically deregistered and its queue is marked as complete.
 *
 * The scheduling strategy is as follows:
 * - Each client registers with a desired frame rate (fps) and frame count.
 * - The collector maintains a schedule of when each client is next due to receive a frame.
 * - When the earliest scheduled time arrives (or is within the allowed slack), the collector
 *   captures a single frame and distributes it to all clients whose schedules fall within the
 *   slack window.
 * - This batching reduces redundant camera captures and ensures efficient resource usage.
 * - The collector supports dynamic registration and deregistration of clients at runtime.
 *
 * @note
 * The current scheduling strategy can be extended or modified. For example, one might restrict
 * allowed FPS values to multiples of 0.125s (i.e., FPS = 0.125, 0.25, 0.5, 1, 2, 4, 8, 16).
 * In this approach, the FrameCollector would continuously capture frames at the highest frame
 * rate required by any connected client, without scheduling individual capture times. A delivery
 * selection mechanism could then be implemented to determine which captured frames are delivered
 * to which clients, simplifying the scheduling logic and potentially improving efficiency.
 *
 * Thread safety: All public methods are thread-safe unless otherwise noted.
 */
class FrameCollector
{
public:
  using frame_queue_t = ClientCollectionParams::frame_queue_t;

  /**
   * @brief Utility static function to create a shared pointer to a frame queue.
   *
   * Clients may use this to obtain a queue prior to registering with the collector.
   * @return Shared pointer to a new frame queue.
   * @see irsol::utils::SafeQueue
   * @see irsol::server::handlers::CommandGIHandler
   * @see irsol::server::handlers::CommandGISHandler
   */
  static std::shared_ptr<frame_queue_t> makeQueuePtr();

  /**
   * @brief Slack window for batching frame delivery to clients.
   *
   * Clients whose scheduled delivery times fall within this window of a captured frame
   * will all receive the same frame. This reduces redundant captures and improves efficiency.
   */
  constexpr static irsol::types::duration_t SLACK = std::chrono::milliseconds(50);

  /**
   * @brief Constructs a FrameCollector for the given camera interface.
   *
   * @param camera Reference to a camera interface for capturing frames.
   */
  FrameCollector(irsol::camera::Interface& camera);

  /**
   * @brief Destructor. Stops any running threads and cleans up resources.
   */
  ~FrameCollector();

  /**
   * @brief Starts the frame collection and distribution thread.
   *
   * If the collector is already running, this call is ignored.
   */
  void start();

  /**
   * @brief Stops the frame collector and joins worker threads.
   *
   * After calling stop(), no further frames will be delivered to clients.
   */
  void stop();

  /**
   * @brief Checks whether the collector is currently serving any clients.
   *
   * @return true if at least one client is registered, false otherwise.
   */
  bool isBusy() const;

  /**
   * @brief Registers a client to receive frames at a specified frame rate.
   *
   * @param clientId    Unique identifier for the client.
   * @param fps         Desired frame rate (frames per second). If fps <= 0 and frameCount == 1,
   *                    the client will receive a single frame immediately. Tipically used in `gi`
   * command @see irsol::server::handlers::CommandGIHandler.
   * @param queue       Shared pointer to the client's frame queue. The collector will push frame
   *                    data into this queue at the schedule defined by the client.
   * @param frameCount  Number of frames to deliver; -1 means unlimited. After the last frame has
   *                    been pushed to the client, the client is automatically deregistered and its
   *                    queue marked as complete.
   * @note This method is thread-safe.
   */
  void registerClient(
    irsol::types::client_id_t      clientId,
    double                         fps,
    std::shared_ptr<frame_queue_t> queue,
    int64_t                        frameCount = -1);

  /**
   * @brief Deregisters a client and stops frame delivery.
   *
   * @param clientId The client's unique identifier.
   *
   * @note Removes all state registered associated with the client in the collector.
   * @note This method is thread-safe.
   */
  void deregisterClient(irsol::types::client_id_t clientId);

private:
  /**
   * @brief Runs the frame distribution loop in a background thread.
   *
   * This method monitors client schedules, captures frames as needed, and distributes them
   * to all clients whose scheduled delivery times fall within the current slack window.
   * It also handles automatic deregistration of clients who have received their requested
   * number of frames.
   */
  void run();

  /**
   * @brief Deregisters a client and stops frame delivery (not thread-safe).
   *
   * @param clientId The client's unique identifier.
   *
   * @note Removes all state registered associated with the client in the collector.
   * @note This method is NOT thread-safe and should only be called with appropriate locking.
   */
  void deregisterClientNonThreadSafe(irsol::types::client_id_t clientId);

  /**
   * @brief Collects clients who are scheduled to receive a frame at the given time.
   *
   * @param now   Current timestamp.
   * @param slack Allowed slack between now and client's schedule for considering a client to be
   *              ready for receiving data.
   * @return A pair of vectors: the first is a list of ready client IDs, the second contains the
   *         actual schedule times of the returned clients.
   */
  std::pair<std::vector<irsol::types::client_id_t>, std::vector<irsol::types::timepoint_t>>
  collectReadyClients(irsol::types::timepoint_t now, irsol::types::duration_t slack) const;

  /**
   * @brief Removes schedules from the schedule map.
   *
   * @param schedules Vector of schedule times to clean up.
   */
  void cleanUpSchedule(const std::vector<irsol::types::timepoint_t> schedules);

  /**
   * @brief Captures an image and returns it along with associated metadata.
   *
   * @return A pair containing frame metadata and the image data buffer if image acquisition was
   *         successful, `std::nullopt` otherwise.
   */
  std::optional<std::pair<FrameMetadata, std::vector<irsol::types::byte_t>>> grabImageData() const;

  /**
   * @brief Schedules the next frame delivery for a client.
   *
   * @param clientId     The client to schedule.
   * @param nextFrameDue The next timestamp when the client should receive a frame.
   * @return true if successfully scheduled, false if the client is no longer active and can later
   *         be removed from the frame collector.
   */
  bool schedule(const irsol::types::client_id_t& clientId, irsol::types::timepoint_t nextFrameDue);

  irsol::camera::Interface& m_cam;  ///< Reference to the camera interface used for capturing.

  std::mutex m_clientsMutex;  ///< Protects access to m_clients and m_scheduleMap.
  std::unordered_map<irsol::types::client_id_t, ClientCollectionParams>
    m_clients;  ///< Stores parameters for each registered client.
  std::map<irsol::types::timepoint_t, std::vector<irsol::types::client_id_t>>
    m_scheduleMap;  ///< Maps timestamps to client IDs scheduled at that time.

  std::condition_variable m_scheduleCondition;  ///< Signals when a new client is scheduled.
  std::thread             m_distributorThread;  ///< Thread responsible for frame distribution.

  std::atomic<bool> m_stop{
    false};  ///< Indicates whether the collector is stopping due to an external request.
};

}  // namespace frame_collector
}  // namespace server
}  // namespace irsol
