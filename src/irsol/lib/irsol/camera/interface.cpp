#include "irsol/camera/interface.hpp"
#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"

namespace irsol {

CameraInterface::CameraInterface() : m_cam(utils::loadDefaultCamera()) {}

NeoAPI::Cam &CameraInterface::getNeoCam() { return m_cam; }

NeoAPI::Image CameraInterface::captureImage() {
  std::lock_guard<std::mutex> lock(m_mutex);
  return m_cam.GetImage();
}

} // namespace irsol