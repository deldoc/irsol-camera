#pragma once

#include "irsol/camera/interface.hpp"

namespace irsol {
class CameraFeatureDiscovery {

public:
  CameraFeatureDiscovery(CameraInterface &cam);
  void run();

private:
  CameraInterface &m_cam;
};
} // namespace irsol