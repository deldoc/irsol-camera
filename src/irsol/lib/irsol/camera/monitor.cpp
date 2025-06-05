#include "irsol/camera/monitor.hpp"

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/types.hpp"

namespace irsol {
namespace camera {
StatusMonitor::StatusMonitor(const Interface& cam, irsol::types::duration_t monitorInterval)
  : m_cam(cam), m_monitorInterval(monitorInterval), m_hasStartedMonitor(false)
{
  IRSOL_ASSERT_ERROR(cam.isConnected(), "Camera is not connected.");
}

StatusMonitor::~StatusMonitor()
{
  if(m_hasStartedMonitor) {
    IRSOL_NAMED_LOG_DEBUG("status_monitor", "Automatic stopping monitoring of camera.");
    stop();
  }
}

void
StatusMonitor::runMonitor() const
{
  IRSOL_NAMED_LOG_DEBUG("status_monitor", "Monitoring camera status...");

  while(m_hasStartedMonitor) {
    auto nextIterationTime = irsol::types::clock_t::now() + m_monitorInterval;

    bool stopRequested = false;
    IRSOL_NAMED_LOG_INFO("status_monitor", "\n{}", m_cam.cameraStatusAsString());
    if(stopRequested) {
      break;
    }

    std::this_thread::sleep_until(nextIterationTime);
  }
}

void
StatusMonitor::start()
{
  IRSOL_ASSERT_ERROR(!m_hasStartedMonitor.load(), "Monitor is already running!");
  m_hasStartedMonitor.store(true);

  m_monitorThread = std::thread([this]() { runMonitor(); });
  IRSOL_NAMED_LOG_DEBUG("status_monitor", "Camera monitor has started.");
}

void
StatusMonitor::stop()
{
  IRSOL_ASSERT_ERROR(
    m_hasStartedMonitor.load(), "Cannot 'stop' monitor without having started it before!");
  m_hasStartedMonitor.store(false);

  if(m_monitorThread.joinable()) {
    m_monitorThread.join();
  }
  IRSOL_NAMED_LOG_DEBUG("status_monitor", "Camera monitor has stopped.");
}
}  // namespace camera
}  // namespace irsol