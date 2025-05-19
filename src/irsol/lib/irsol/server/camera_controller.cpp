#include "irsol/server/camera_controller.hpp"
#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"

namespace irsol {

CameraController::CameraController() : m_cam(utils::loadDefaultCamera()) {}

std::string CameraController::getParam(const std::string &param) {
  std::lock_guard<std::mutex> lock(m_mutex);
  IRSOL_LOG_DEBUG("Getting parameter '{}'", param);
  // Use NeoAPI to get parameter value
  try {
    NeoAPI::NeoString neoParam(param.c_str());
    auto paramValue = m_cam.GetFeature(neoParam);
    return NeoAPI::NeoString(paramValue).c_str();
  } catch (const std::exception &e) {
    IRSOL_LOG_ERROR("Failed to get parameter '{}': {}", param, e.what());
    return "";
  }
}

void CameraController::setParam(const std::string &param, const std::string &value) {
  std::lock_guard<std::mutex> lock(m_mutex);
  IRSOL_LOG_DEBUG("Setting parameter '{}' to '{}'", param, value);
  // Use NeoAPI to set value
  try {
    NeoAPI::NeoString neoParam(param.c_str());
    NeoAPI::NeoString neoValue(value.c_str());
    m_cam.SetFeature(neoParam, neoValue);
  } catch (const std::exception &e) {
    IRSOL_LOG_ERROR("Failed to set parameter '{}': {}", param, e.what());
  }
}

std::string CameraController::captureImage() {
  std::lock_guard<std::mutex> lock(m_mutex);
  // Use NeoAPI to grab image, save to disk
  return "image.jpg";
}
} // namespace irsol