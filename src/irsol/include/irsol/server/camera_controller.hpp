#pragma once
#include "neoapi/neoapi.hpp"
#include <mutex>
#include <string>

namespace irsol {
class CameraController {
public:
  CameraController();
  std::string getParam(const std::string &param);
  void setParam(const std::string &param, const std::string &value);
  std::string captureImage(); // Returns path or base64

private:
  std::mutex m_mutex;
  NeoAPI::Cam m_cam; // NeoAPI camera object
};
} // namespace irsol