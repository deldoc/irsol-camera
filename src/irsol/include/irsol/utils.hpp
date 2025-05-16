#pragma once

#include "neoapi/neoapi.hpp"
#include <string>
#include <vector>

namespace irsol {
namespace utils {
std::vector<std::string> split(const std::string &s, char delimiter);
std::string strip(const std::string &s, const std::string &delimiters = " \t\r\n ");
NeoAPI::Cam loadDefaultCamera();
void logCameraInfo(const NeoAPI::CamInfo &info);
NeoAPI::CamInfoList &discoverCameras();
namespace internal {
constexpr const char *defaultCameraSerialNumber();
}
} // namespace utils
} // namespace irsol