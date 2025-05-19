#pragma once

#include "neoapi/neoapi.hpp"
#include <string>
#include <vector>

namespace irsol {
namespace utils {
std::string uuid();
std::vector<std::string> split(const std::string &s, char delimiter);
std::string strip(const std::string &s, const std::string &delimiters = " \t\r\n ");
std::string stripString(const std::string &s, const std::string &strippedString);
NeoAPI::Cam loadDefaultCamera();
void logCameraInfo(const NeoAPI::CamInfo &info);
NeoAPI::CamInfoList &discoverCameras();
namespace internal {
constexpr const char *defaultCameraSerialNumber();
}
} // namespace utils
} // namespace irsol