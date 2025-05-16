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

void CameraStatusMonitor::runMonitor() const {
  IRSOL_LOG_DEBUG("Monitoring camera status...");
  static const char *const FEATURE_NAMES[] = {"AcquisitionFrameCount",
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
    auto nextIterationTime = std::chrono::steady_clock::now() + m_monitorInterval;

    tabulate::Table featureResults;
    featureResults.add_row({"Feature", "Value"});

    bool stopRequested = false;

    for (const auto featureName : FEATURE_NAMES) {
      auto feature = m_cam.GetFeature(featureName);
      auto featureValue = NeoAPI::NeoString(feature).c_str();
      featureResults.add_row({featureName, featureValue});
      IRSOL_LOG_TRACE("{0:s}: {1:s}", featureName, featureValue);
      if (!m_hasStartedMonitor) {
        stopRequested = true;
        break;
      }
    }
    featureResults.column(0).format().font_align(tabulate::FontAlign::right);
    IRSOL_LOG_INFO("\n{0:s}", featureResults.str());

    if (stopRequested) {
      break;
    }

    std::this_thread::sleep_until(nextIterationTime);
  }
}

void CameraStatusMonitor::start() {
  std::lock_guard<std::mutex> guard(m_startStopMutex);
  IRSOL_ASSERT_ERROR(!m_hasStartedMonitor, "Monitor is already running!");
  m_hasStartedMonitor = true;

  m_monitorThread = std::thread([this]() { runMonitor(); });
  IRSOL_LOG_DEBUG("Camera monitor has started.");
}

void CameraStatusMonitor::stop() {
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