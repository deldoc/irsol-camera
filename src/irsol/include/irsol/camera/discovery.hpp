#pragma once

#include "irsol/camera/interface.hpp"

namespace irsol {
namespace camera {
class FeatureDiscovery {

public:
  FeatureDiscovery(Interface &cam);
  void run();

private:
  Interface &m_cam;
};
} // namespace camera
} // namespace irsol