#include "irsol/camera/monitor.hpp"

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/types.hpp"

namespace irsol {
namespace camera {
StatusMonitor::StatusMonitor(const Interface& cam, std::chrono::milliseconds monitorInterval)
  : m_cam(cam), m_monitorInterval(monitorInterval), m_hasStartedMonitor(false)
{
  IRSOL_ASSERT_ERROR(cam.isConnected(), "Camera is not connected.");
}

StatusMonitor::~StatusMonitor()
{
  if(m_hasStartedMonitor) {
    IRSOL_LOG_DEBUG("Automatic stopping monitoring of camera.");
    stop();
  }
}

void
StatusMonitor::runMonitor() const
{
  IRSOL_LOG_DEBUG("Monitoring camera status...");

  while(m_hasStartedMonitor) {
    auto nextIterationTime = irsol::types::clock_t::now() + m_monitorInterval;

    bool stopRequested = false;
    IRSOL_LOG_INFO("\n{}", m_cam.cameraStatusAsString());
    if(stopRequested) {
      break;
    }

    std::this_thread::sleep_until(nextIterationTime);
  }
}

void
StatusMonitor::start()
{
  std::scoped_lock<std::mutex> guard(m_startStopMutex);
  IRSOL_ASSERT_ERROR(!m_hasStartedMonitor, "Monitor is already running!");
  m_hasStartedMonitor = true;

  m_monitorThread = std::thread([this]() { runMonitor(); });
  IRSOL_LOG_DEBUG("Camera monitor has started.");
}

void
StatusMonitor::stop()
{
  std::scoped_lock<std::mutex> guard(m_startStopMutex);
  IRSOL_ASSERT_ERROR(
    m_hasStartedMonitor, "Cannot 'stop' monitor without having started it before!");
  m_hasStartedMonitor = false;

  if(m_monitorThread.joinable()) {
    m_monitorThread.join();
  }
  IRSOL_LOG_DEBUG("Camera monitor has stopped.");
}
}  // namespace camera
}  // namespace irsol