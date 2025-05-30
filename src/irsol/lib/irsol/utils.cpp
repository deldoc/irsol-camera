#include "irsol/utils.hpp"

#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "neoapi/neoapi.hpp"
#include "tabulate/tabulate.hpp"

#include <iomanip>
#include <random>
#include <sstream>
#include <thread>

namespace irsol {
namespace utils {
std::string
uuid()
{
  static std::random_device       rd;
  static std::mt19937             gen(rd());
  std::uniform_int_distribution<> dis(0, 15);
  std::uniform_int_distribution<> dis2(8, 11);

  std::stringstream ss;
  ss << std::hex;

  for(int i = 0; i < 8; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for(int i = 0; i < 4; i++) {
    ss << dis(gen);
  }
  ss << "-4";  // Version 4
  for(int i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  ss << dis2(gen);
  for(int i = 0; i < 3; i++) {
    ss << dis(gen);
  }
  ss << "-";
  for(int i = 0; i < 12; i++) {
    ss << dis(gen);
  }
  return ss.str();
}

std::vector<std::string>
split(const std::string& s, char delimiter)
{
  std::vector<std::string> tokens;
  std::string              token;
  for(char c : s) {
    if(c == delimiter) {
      if(!token.empty()) {
        tokens.push_back(token);
        token.clear();
      }
    } else {
      token += c;
    }
  }
  if(!token.empty()) {
    tokens.push_back(token);
  }
  return tokens;
}

std::string
strip(const std::string& s, const std::string& delimiters)
{
  size_t start = 0;
  size_t end   = s.size();
  IRSOL_LOG_TRACE("Stripping delimiters '{0:s}' from string '{1:s}'", delimiters, s);
  while((start < end) && (delimiters.find(s[start]) != std::string::npos)) {
    start++;
  }
  while((start < end) && (delimiters.find(s[end - 1]) != std::string::npos)) {
    end--;
  }

  auto result = s.substr(start, end - start);
  IRSOL_LOG_TRACE("Stripped string '{0:s}' is '{1:s}'", s, result);
  return result;
}

std::string
stripString(const std::string& s, const std::string& strippedString)
{
  // Check if the stripped string is present in the input string at the beginning
  std::string result{s};
  IRSOL_LOG_TRACE("Stripping string '{0:s}' from string '{1:s}'", strippedString, s);

  if(result.find(strippedString) == 0) {
    result = result.substr(strippedString.length());
  }

  // Check if the stripped string is present in the input string at the end
  if(result.rfind(strippedString) == result.size() - strippedString.size()) {
    result = result.substr(0, result.size() - strippedString.size());
  }

  IRSOL_LOG_TRACE(
    "Stripped string '{0:s}' from string '{1:s}' is '{2:s}'", s, strippedString, result);

  return result;
}

std::string
timestampToString(irsol::types::timepoint_t tp)
{
  using system_clock_t = std::chrono::system_clock;
  auto now_sys         = std::chrono::time_point_cast<system_clock_t::duration>(
    tp - irsol::types::clock_t::now() + system_clock_t::now());

  // Extract microseconds from time since epoch
  auto us = std::chrono::duration_cast<std::chrono::microseconds>(now_sys.time_since_epoch()) %
            std::chrono::seconds(1);

  // Convert to time_t for formatting
  std::time_t t_c = system_clock_t::to_time_t(now_sys);
  std::tm     tm  = *std::localtime(&t_c);

  std::stringstream ss;
  ss << std::put_time(&tm, "%F %T");                             // Date and time to seconds
  ss << '.' << std::setfill('0') << std::setw(6) << us.count();  // Append microseconds

  return ss.str();
}

std::string
durationToString(irsol::types::duration_t dr)
{
  std::stringstream ss;
  if(dr.count() == 0) {
    return "0s";
  }

  if(dr >= std::chrono::hours(1)) {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(dr);
    ss << std::to_string(hours.count());
    ss << "hours ";
    dr -= hours;
  }
  if(dr >= std::chrono::minutes(1)) {
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(dr);
    ss << std::setw(2) << std::setfill('0') << std::to_string(minutes.count());
    ss << "minutes ";
    dr -= minutes;
  }
  if(dr >= std::chrono::seconds(1)) {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dr);
    ss << std::setw(2) << std::setfill('0') << std::to_string(seconds.count());
    ss << "s ";
    dr -= seconds;
  }
  if(dr >= std::chrono::milliseconds(1)) {
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(dr);
    ss << std::setw(3) << std::setfill('0') << std::to_string(milliseconds.count());
    ss << "ms ";
    dr -= milliseconds;
  }
  if(dr >= std::chrono::microseconds(1)) {
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(dr);
    ss << std::setw(3) << std::setfill('0') << std::to_string(microseconds.count());
    ss << "us ";
    dr -= microseconds;
  }
  if(dr >= std::chrono::nanoseconds(1)) {
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(dr);
    ss << std::setw(3) << std::setfill('0') << std::to_string(nanoseconds.count());
    ss << "ns ";
    dr -= nanoseconds;
  }

  auto result = ss.str();
  return strip(result, " 0");
}

NeoAPI::Cam
loadDefaultCamera()
{
  IRSOL_LOG_DEBUG("Loading default camera");
  NeoAPI::Cam cam = NeoAPI::Cam();

  const auto cameraSerialNumber = internal::defaultCameraSerialNumber();
  IRSOL_LOG_TRACE("Trying to connect to default camera with SN '{0:s}'.", cameraSerialNumber);
  try {
    cam.Connect(cameraSerialNumber);
  } catch(NeoAPI::NotConnectedException& e) {
    IRSOL_ASSERT_FATAL(false, "Camera connection failed: %s", e.GetDescription());
    throw e;
  }
  IRSOL_ASSERT_FATAL(cam.IsConnected(), "Camera is not connected");
  IRSOL_LOG_DEBUG("Camera connection successful");

  return cam;
}

void
logCameraInfo(const NeoAPI::CamInfo& info)
{
  const auto model          = info.GetModelName();
  const auto camId          = info.GetId();
  const auto serial         = info.GetSerialNumber();
  const auto tlType         = info.GetTLType();
  const auto vendor         = info.GetVendorName();
  const auto usb3VisionGuid = info.GetUSB3VisionGUID();
  const auto usbPortId      = info.GetUSBPortID();
  const auto gevIpAddress   = info.GetGevIpAddress();
  const auto gevSubnetMask  = info.GetGevSubnetMask();
  const auto gevGateway     = info.GetGevGateway();
  const auto gevMacAddress  = info.GetGevMACAddress();

  tabulate::Table camInfo;
  camInfo.add_row({"Name", "Value"});
  camInfo.add_row({"Camera Model Name", model.c_str()});
  camInfo.add_row({"Camera ID", camId.c_str()});
  camInfo.add_row({"Camera Serial Number", serial.c_str()});
  camInfo.add_row({"Camera Transport Layer Type", tlType.c_str()});
  camInfo.add_row({"Camera Vendor Name", vendor.c_str()});
  camInfo.add_row({"Camera USB3 Vision GUID", usb3VisionGuid.c_str()});
  camInfo.add_row({"Camera USB Port ID", usbPortId.c_str()});
  camInfo.add_row({"Camera GEV IP Address", gevIpAddress.c_str()});
  camInfo.add_row({"Camera GEV Subnet Mask", gevSubnetMask.c_str()});
  camInfo.add_row({"Camera GEV Gateway", gevGateway.c_str()});
  camInfo.add_row({"Camera GEV MAC Address", gevMacAddress.c_str()});
  camInfo.add_row({"Is connectable", info.IsConnectable() ? "true" : "false"});

  camInfo.column(0).format().font_align(tabulate::FontAlign::right);
  IRSOL_LOG_INFO("\n{0:s}", camInfo.str());
}

NeoAPI::CamInfoList&
discoverCameras()
{
  IRSOL_LOG_TRACE("Discovering cameras");
  NeoAPI::CamInfoList& infoList = NeoAPI::CamInfoList::Get();
  IRSOL_LOG_TRACE("Refreshing camera list");
  infoList.Refresh();

  return infoList;
}
namespace internal {
constexpr const char*
defaultCameraSerialNumber()
{
  return "700011810487";
}
}  // namespace internal

}  // namespace utils
}  // namespace irsol