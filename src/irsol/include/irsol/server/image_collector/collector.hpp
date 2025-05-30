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
   * @brief Collects clients who are scheduled to receive a frame at the given time.
   *
   * @param now Current timestamp.
   * @return List of client IDs ready to receive a frame.
   *
   * @note Updates the m_scheduleMap by removing the schedule for the clients that are collected as
   * ready.
   */
  std::vector<irsol::types::client_id_t> collectReadyClients(irsol::types::timepoint_t now);

  /**
   * @brief Captures an image and returns it along with associated metadata.
   *
   * @return A pair containing frame metadata and the image data buffer.
   */
  std::pair<FrameMetadata, std::vector<irsol::types::byte_t>> grabImageData() const;

  /**
   * @brief Computes the first frame due time for a newly registered client.
   *
   * @param interval The client's desired frame interval.
   * @param maxDiff  Maximum allowable skew from aligned frame times. This is to avoid situations
   * where one client is registered in a "slow" pool of clients, and for it not to be scheduled with
   * all other slow clients.
   * @return The computed time point of the next due frame for the new client.
   */
  irsol::types::timepoint_t computeNextDueTimeForNewClient(
    irsol::types::duration_t interval,
    irsol::types::duration_t maxDiff) const;

  /**
   * @brief Schedules the next frame delivery for a client.
   *
   * @param clientId     The client to schedule.
   * @param nextFrameDue The next timestamp when the client should receive a frame.
   * @return true if successfully scheduled, false if the client is no longer active and can later
   * be removed from the frame collector.
   */
  bool schedule(const irsol::types::client_id_t clientId, irsol::types::timepoint_t nextFrameDue);

  irsol::camera::Interface& m_cam;  ///< Reference to the camera interface used for capturing.

  std::mutex m_clientsMutex;  ///< Protects access to m_clients and m_scheduleMap.
  std::unordered_map<irsol::types::client_id_t, ClientCollectionParams>
    m_clients;  ///< Stores parameters for each registered client.
  std::map<irsol::types::timepoint_t, std::vector<irsol::types::client_id_t>>
    m_scheduleMap;  ///< Maps timestamps to client IDs scheduled at that time.

  std::condition_variable m_scheduleCondition;  ///< Signals when a new client is scheduled.
  std::thread             m_distributorThread;  ///< Thread responsible for frame distribution.

  std::atomic<bool> m_stop{false};  ///< Indicates whether the collector is stopping.
};

}  // namespace frame_collector
}  // namespace server
}  // namespace irsol
