#include "irsol/camera/interface.hpp"

#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"

namespace irsol {
namespace camera {

Interface::Interface(NeoAPI::Cam cam): m_cam(cam)
{

  // Set the mode to manual
  IRSOL_LOG_INFO("Setting camera mode to 'TriggerMode::On'.");
  setParam("TriggerMode", "On");
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
  interface.setParam("BinningVertical", 1);
  interface.setParam("BinningVerticalMode", "Sum");
  interface.setParam("BinningHorizontal", 1);
  interface.setParam("BinningHorizontalMode", "Sum");
  return interface;
}

Interface
Interface::HalfResolution()
{
  auto cam = ::irsol::utils::loadDefaultCamera();

  Interface interface(cam);
  interface.setParam("BinningVertical", 2);
  interface.setParam("BinningVerticalMode", "Average");
  interface.setParam("BinningHorizontal", 2);
  interface.setParam("BinningHorizontalMode", "Average");
  return interface;
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