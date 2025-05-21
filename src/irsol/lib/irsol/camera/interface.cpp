#include "irsol/camera/interface.hpp"

#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"

namespace irsol {
namespace camera {

Interface::Interface(): m_cam(utils::loadDefaultCamera())
{

  // Set the mode to manual
  IRSOL_LOG_INFO("Setting camera mode to 'TriggerMode::On'.");
  setParam("TriggerMode", "On");
}

NeoAPI::Cam&
Interface::getNeoCam()
{
  return m_cam;
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

NeoAPI::Image
Interface::captureImage(std::chrono::milliseconds timeout)
{
  std::lock_guard<std::mutex> lock(m_imageMutex);
  m_cam.f().TriggerSoftware.Execute();  // execute a software trigger to get an image
  return m_cam.GetImage(
    static_cast<uint32_t>(timeout.count()));  // retrieve the image to work with it
}
}  // namespace camera
}  // namespace irsol