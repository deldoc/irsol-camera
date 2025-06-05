/**
 * @file irsol/camera/monitor.hpp
 * @brief Defines the StatusMonitor class to periodically monitor camera status in a background
 * thread.
 *
 * This header declares the StatusMonitor class, which enables monitoring of camera status or
 * other relevant metrics by running a periodic background task on a separate thread.
 * The monitoring interval can be customized via a std::chrono::milliseconds parameter.
 *
 * The class manages starting and stopping the monitoring thread safely,
 * allowing derived classes to override the monitoring behavior by implementing runMonitor().
 *
 * Usage:
 * ```cpp
 * irsol::camera::Interface cam = ...;
 * StatusMonitor monitor(cam, std::chrono::milliseconds(200));
 * monitor.start();
 * // do other work...
 * monitor.stop();
 * ```
 */

#pragma once

#include "irsol/camera/interface.hpp"
#include "irsol/types.hpp"

#include <atomic>
#include <chrono>
#include <thread>

namespace irsol {
namespace camera {

/**
 * @brief Class that offers an easy way to run a background monitorin thread that periodically
 * inspects and reports information about the camera interface passed as argument.
 *
 * @see irsol::camera::Interface
 *
 * Check out @ref loading_images for an example usage.
 */
class StatusMonitor
{
public:
  /**
   * @brief Constructs a StatusMonitor for the given camera interface.
   * @param cam Reference to the camera interface to monitor.
   * @param monitorInterval Interval at which the monitoring task should run. Defaults to 100ms.
   */
  StatusMonitor(
    const Interface&         cam,
    irsol::types::duration_t monitorInterval = std::chrono::milliseconds(100));

  /**
   * @brief Destructor stops the monitoring thread if running.
   */
  ~StatusMonitor();

  /**
   * @brief Starts the monitoring thread.
   * If the monitor is already running, this call has no effect.
   */
  void start();

  /**
   * @brief Stops the monitoring thread and waits for it to finish.
   * If the monitor is not running, this call has no effect.
   */
  void stop();

protected:
  /**
   * @brief The main monitoring loop executed on the background thread.
   *
   * Override this method in derived classes to implement specific monitoring logic.
   * It is called repeatedly with the frequency specified by monitorInterval.
   */
  virtual void runMonitor() const;

private:
  /// Reference to the camera interface being monitored
  const irsol::camera::Interface& m_cam;

  /// Interval at which the monitor thread executes runMonitor()
  irsol::types::duration_t m_monitorInterval;

  /// Flag indicating if the monitor thread has been started
  std::atomic<bool> m_hasStartedMonitor;

  /// Thread running the periodic monitoring loop
  std::thread m_monitorThread;
};

}  // namespace camera
}  // namespace irsol
