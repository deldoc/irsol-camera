#include "irsol/monitor.hpp"
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "tabulate/tabulate.hpp"

namespace irsol {
CameraStatusMonitor::CameraStatusMonitor(const NeoAPI::Cam &cam,
                                         std::chrono::milliseconds monitorInterval)
    : m_cam(cam), m_monitorInterval(monitorInterval), m_hasStartedMonitor(false) {
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
                                              "TriggerMode",
                                              "PayloadSize",
                                              "PixelFormat",
                                              "ReadOutTime"};
  while (m_hasStartedMonitor) {
    auto next_iteration_time = std::chrono::steady_clock::now() + m_monitorInterval;

    tabulate::Table feature_results;
    feature_results.add_row({"Feature", "Value"});

    bool stop_requested = false;

    for (const auto feature_name : feature_names) {
      auto feature = m_cam.GetFeature(feature_name);
      auto feature_value = NeoAPI::NeoString(feature).c_str();
      feature_results.add_row({feature_name, feature_value});
      IRSOL_LOG_TRACE("{0:s}: {1:s}", feature_name, feature_value);
      if (!m_hasStartedMonitor) {
        stop_requested = true;
        break;
      }
    }
    feature_results.column(0).format().font_align(tabulate::FontAlign::right);
    IRSOL_LOG_INFO("\n{0:s}", feature_results.str());

    if (stop_requested) {
      break;
    }

    std::this_thread::sleep_until(next_iteration_time);
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