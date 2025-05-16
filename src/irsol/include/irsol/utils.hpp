#pragma once

#include "neoapi/neoapi.hpp"

namespace irsol {
namespace utils {
NeoAPI::Cam loadDefaultCamera();
void logCameraInfo(const NeoAPI::CamInfo &info);
NeoAPI::CamInfoList &discoverCameras();
namespace internal {
constexpr const char *defaultCameraSerialNumber();
}
} // namespace utils
} // namespace irsol