#include "irsol/monitor.hpp"
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "tabulate/tabulate.hpp"

namespace irsol {
CameraStatusMonitor::CameraStatusMonitor(const NeoAPI::Cam &cam)
    : m_cam(cam), m_hasStartedMonitor(false) {
  IRSOL_ASSERT_ERROR(cam.IsConnected(), "Camera is not connected.");
}

CameraStatusMonitor::~CameraStatusMonitor() {
  if (m_hasStartedMonitor) {
    IRSOL_LOG_DEBUG("Automatic stopping monitoring of camera.");
    stop();
  }
}

void CameraStatusMonitor::run_monitor() const {
  IRSOL_LOG_DEBUG("Monitoring camera status...");
  static const char *const feature_names[] = {"AcquisitionFrameCount",
                                              "AcquisitionFrameRate",
                                              "AcquisitionMode",
                                              "DeviceTemperatureStatus",
                                              "DeviceTemperature",
                                              "ExposureMode",
                                              "ExposureTime",
                                              "FrameCounter",
                                              "Height",
                                              "Width",
                                              "PayloadSize",
                                              "PixelFormat",
                                              "ReadOutTime"};
  while (m_hasStartedMonitor) {
    tabulate::Table feature_results;
    feature_results.add_row({"Feature", "Value"});
    for (const auto feature_name : feature_names) {
      auto feature = m_cam.GetFeature(feature_name);
      auto feature_desc = NeoAPI::NeoString(feature).c_str();
      feature_results.add_row({feature_name, feature_desc});
      IRSOL_LOG_TRACE("{0:s}: {1:s}", feature_name, feature_desc);
      if (!m_hasStartedMonitor) {
        break;
      }
    }
    feature_results.column(0).format().font_align(tabulate::FontAlign::right);
    IRSOL_LOG_INFO("\n{0:s}", feature_results.str());
  }
}

void CameraStatusMonitor::start() {
  // acquire the mutex
  std::lock_guard<std::mutex> guard(m_startStopMutex);
  IRSOL_ASSERT_ERROR(!m_hasStartedMonitor, "Monitor is already running!");
  m_hasStartedMonitor = true;

  // start the thread with the monitoring pipeline
  m_monitorThread = std::thread([this]() { run_monitor(); });
  IRSOL_LOG_DEBUG("Camera monitor has started.");
}

void CameraStatusMonitor::stop() {
  // acquire the mutex
  std::lock_guard<std::mutex> guard(m_startStopMutex);
  IRSOL_ASSERT_ERROR(m_hasStartedMonitor,
                     "Cannot 'stop' monitor without having started it before!");
  m_hasStartedMonitor = false;

  if (m_monitorThread.joinable()) {
    m_monitorThread.join();
  }
  IRSOL_LOG_DEBUG("Camera monitor has stopped.");
}
} // namespace irsol