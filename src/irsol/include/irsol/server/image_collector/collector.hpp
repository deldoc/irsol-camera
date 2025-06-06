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

namespace irsol {
namespace server {
namespace frame_collector {

/**
 * @brief FrameCollector coordinates frame acquisition from a camera and distributes frames
 *        to registered clients based on their requested frame rates.
 */
class FrameCollector
{
public:
  using frame_queue_t = ClientCollectionParams::frame_queue_t;

  /**
   * Utility static function to create a shared pointer to a queue that a client might want to use
   * prior to registering to the collector.
   */
  static std::shared_ptr<frame_queue_t> makeQueuePtr();

  /**
   * A slack representing the timewindow around the captured image timestamp for
   * selecting the clients (based on their schedule) to which send the image.
   *
   * This allows the FrameCollector to capture a single image, and distribute it to many clients
   * as long as their desired due time is within the slack from the capture time.
   */
  constexpr static irsol::types::duration_t SLACK = std::chrono::milliseconds(50);

  /**
   * @brief Constructs a FrameCollector for the given camera interface.
   *
   * @param camera Reference to a camera interface for capturing frames.
   */
  FrameCollector(irsol::camera::Interface& camera);

  /**
   * @brief Destructor. Stops any running threads and cleans up.
   */
  ~FrameCollector();

  /**
   * @brief Starts the frame collection and distribution threads.
   */
  void start();

  /**
   * @brief Stops the frame collector and joins worker threads.
   */
  void stop();

  /**
   * @brief Checks whether the collector is currently running.
   *
   * @return true if at least one client is registered, false otherwise.
   */
  bool isBusy() const;

  /**
   * @brief Registers a client to receive frames at a specified frame rate.
   *
   * @param clientId    Unique identifier for the client.
   * @param fps         Desired frame rate (frames per second).
   * @param queue       Shared pointer to the client's frame queue. The collector will push frame
   * data into this queue at the schedule defined by the client.
   * @param frameCount  Number of frames to deliver; -1 means unlimited. After the last frame has
   * been pushed to the client, the client is automatically de-registered and its queue marked as
   * complete.
   * @note This method is threadsafe.
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
   * @note This method is threadsafe.
   */
  void deregisterClient(irsol::types::client_id_t clientId);

private:
  /**
   * @brief Runs the frameDistributor. Usually called as a separated thread.
   *
   * Orchestrates the image grabbing and frame-data distribution to clients via their queues.
   */
  void run();

  /**
   * @brief Deregisters a client and stops frame delivery.
   *
   * @param clientId The client's unique identifier.
   *
   * @note Removes all state registered associated with the client in the collector.
   * @note This method is NOT threadsafe.
   */
  void deregisterClientNonThreadSafe(irsol::types::client_id_t clientId);

  /**
   * @brief Collects clients who are scheduled to receive a frame at the given time.
   *
   * @param now Current timestamp.
   * @param slack Allowed slack between now and client's schedule for considering a client to be
   * ready for receiving data.
   * @return A pair of vectors, the first being a list of ready clientIds, the second containing the
   * actual scheduleTimes of the returned clients.
   */

  std::pair<std::vector<irsol::types::client_id_t>, std::vector<irsol::types::timepoint_t>>
  collectReadyClients(irsol::types::timepoint_t now, irsol::types::duration_t slack) const;

  /**
   * @brief Removes schedules older than now
   *
   * @param schedules Vector of schedules to clean up
   */
  void cleanUpSchedule(const std::vector<irsol::types::timepoint_t> schedules);

  /**
   * @brief Captures an image and returns it along with associated metadata.
   *
   * @return A pair containing frame metadata and the image data buffer if image acquisition was
   * successful, `std::nullopt` otherwise.
   */
  std::optional<std::pair<FrameMetadata, std::vector<irsol::types::byte_t>>> grabImageData() const;

  /**
   * @brief Schedules the next frame delivery for a client.
   *
   * @param clientId     The client to schedule.
   * @param nextFrameDue The next timestamp when the client should receive a frame.
   * @return true if successfully scheduled, false if the client is no longer active and can later
   * be removed from the frame collector.
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
