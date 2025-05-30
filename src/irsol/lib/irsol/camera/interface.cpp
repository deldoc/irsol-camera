#include "irsol/camera/interface.hpp"

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"

namespace irsol {
namespace camera {

Interface::Interface(NeoAPI::Cam cam): m_cam(cam)
{

  // Here we configure the camera settings so that we can trigger it on-demand via software
  // triggers.
  IRSOL_LOG_INFO("Configuring camera for manual trigger via software events");
  setMultiParam({{"TriggerMode", {"On"}},               // Enable software trigger
                 {"AcquisitionMode", {"SingleFrame"}},  // Take a single frame, only when triggered
                 {"TriggerSource", {"Software"}}});

  // Here we set the exposure parameters, so that they can be controlled via software
  IRSOL_LOG_INFO("Configuring camera for manual exposure");
  setMultiParam({
    {"ExposureAuto", {"Off"}},   // Disable automatic exposure
    {"ExposureMode", {"Timed"}}  // Exposure mode: timed
  });

  // Store the current exposure of the camera
  m_CachedExposureTime = getExposure();
}

Interface::Interface(Interface&& other): m_cam(other.m_cam) {}
Interface&
Interface::operator=(Interface&& other)
{
  m_cam = other.m_cam;
  return *this;
}

Interface
Interface::FullResolution()
{
  auto cam = ::irsol::utils::loadDefaultCamera();

  Interface interface(cam);
  interface.setMultiParam({{"BinningVertical", {1}},
                           {"BinningVerticalMode", {"Sum"}},
                           {"BinningHorizontal", {1}},
                           {"BinningHorizontalMode", {"Sum"}}});
  interface.resetSensorArea();
  return interface;
}

Interface
Interface::HalfResolution()
{
  auto cam = ::irsol::utils::loadDefaultCamera();

  Interface interface(cam);
  interface.setMultiParam({{"BinningVertical", {2}},
                           {"BinningVerticalMode", {"Average"}},
                           {"BinningHorizontal", {2}},
                           {"BinningHorizontalMode", {"Average"}}});
  interface.resetSensorArea();

  return interface;
}

NeoAPI::Cam&
Interface::getNeoCam()
{
  return m_cam;
}

void
Interface::resetSensorArea()
{
  IRSOL_LOG_INFO("Resetting sensor area");
  int maxWidth  = getParam<int>("WidthMax");
  int maxHeight = getParam<int>("HeightMax");

  setMultiParam(
    {{"Width", {maxWidth}}, {"Height", {maxHeight}}, {"OffsetX", {0}}, {"OffsetY", {0}}});
}

irsol::types::duration_t
Interface::getExposure() const
{
  auto exposureInMicroSeconds = getParam<int64_t>("ExposureTime");
  return std::chrono::microseconds(exposureInMicroSeconds);
}

irsol::types::duration_t
Interface::setExposure(irsol::types::duration_t exposure)
{
  IRSOL_ASSERT_ERROR(exposure.count() > 0, "Cannot set non-positive exposure");
  auto exposureInMicroseconds =
    std::chrono::duration_cast<std::chrono::microseconds>(exposure).count();
  auto setExposureInMicroseconds =
    setParam("ExposureTime", static_cast<int64_t>(exposureInMicroseconds));
  m_CachedExposureTime = std::chrono::microseconds(setExposureInMicroseconds);
  return m_CachedExposureTime;
}

std::string
Interface::getParam(const std::string& param) const
{
  IRSOL_LOG_DEBUG("Getting parameter '{}'", param);
  try {
    NeoAPI::NeoString neoParam(param.c_str());
    auto              feature = m_cam.GetFeature(neoParam);
    return NeoAPI::NeoString(feature).c_str();
  } catch(const std::exception& e) {
    IRSOL_LOG_ERROR("Failed to get parameter '{}': {}", param, e.what());
    return "Unknown";
  }
}

void
Interface::setMultiParam(const std::unordered_map<std::string, camera_param_t>& params)
{
  IRSOL_LOG_DEBUG("Setting multiple parameters");
  std::lock_guard<std::mutex> lock(m_camMutex);
  for(const auto& [param, value] : params) {
    std::visit(
      [&param, this](auto&& arg) {
        using U = std::decay_t<decltype(arg)>;
        IRSOL_LOG_TRACE("Setting parameter '{}' to value '{}'", param, arg);
        setParamNonThreadSafe<U>(param, arg);
      },
      value);
  }
}

void
Interface::trigger(const std::string& param)
{
  IRSOL_LOG_TRACE("Triggering camera with parameter '{}'", param);
  try {
    NeoAPI::NeoString neoParam(param.c_str());
    auto              feature = m_cam.GetFeature(neoParam);
    feature.Execute();
  } catch(const std::exception& e) {
    IRSOL_LOG_ERROR("Failed to trigger camera with parameter '{}': {}", param, e.what());
  }
}

Interface::image_t
Interface::captureImage(std::optional<irsol::types::duration_t> timeout)
{
  std::lock_guard<std::mutex> lock(m_camMutex);

  // Send software trigger to get an image
  trigger("AcquisitionStart");
  trigger("TriggerSoftware");
  // Wait for image, either using the current cached exposure time with a small buffer
  // or using the user-provided timeout
  irsol::types::duration_t actualTimeout = m_CachedExposureTime + std::chrono::milliseconds(50);
  if(timeout.has_value()) {
    IRSOL_LOG_INFO(
      "User provided a custom timeout of capturing camera of {}",
      irsol::utils::durationToString(*timeout));
    actualTimeout = *timeout;
  } else {
    IRSOL_LOG_INFO(
      "Using exposure from camera {} with buffer", irsol::utils::durationToString(actualTimeout));
  }

  uint32_t timeoutMs = static_cast<uint32_t>(
    std::chrono::duration_cast<std::chrono::milliseconds>(actualTimeout).count());
  auto image = m_cam.GetImage(timeoutMs);
  if(image.IsEmpty() || image.GetSize() == 0) {
    IRSOL_LOG_WARN("Timeout or empty image received.");
  }
  trigger("AcquisitionStop");
  return image;
}
}  // namespace camera
}  // namespace irsol