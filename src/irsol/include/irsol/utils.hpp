#pragma once

#include "neoapi/neoapi.hpp"

namespace irsol {
namespace utils {
NeoAPI::Cam load_default_camera();

void log_camera_info(const NeoAPI::CamInfo &info);
NeoAPI::CamInfoList &discover_cameras();
namespace internal {
constexpr const char *default_camera_serial_number();
}
} // namespace utils
} // namespace irsol