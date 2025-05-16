#include "irsol/utils.hpp"
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "neoapi/neoapi.hpp"
#include "tabulate/tabulate.hpp"
#include <thread>
namespace irsol {
namespace utils {

NeoAPI::Cam loadDefaultCamera() {
  IRSOL_LOG_DEBUG("Loading default camera");
  NeoAPI::Cam cam = NeoAPI::Cam();

  const auto cameraSerialNumber = internal::defaultCameraSerialNumber();
  IRSOL_LOG_TRACE("Trying to connect to default camera with SN '{0:s}'.", cameraSerialNumber);
  try {
    cam.Connect(cameraSerialNumber);
  } catch (NeoAPI::NotConnectedException &e) {
    IRSOL_ASSERT_FATAL(false, "Camera connection failed: %s", e.GetDescription());
    throw e;
  }
  IRSOL_ASSERT_FATAL(cam.IsConnected(), "Camera is not connected");
  IRSOL_LOG_DEBUG("Camera connection successful");
  return cam;
}

void logCameraInfo(const NeoAPI::CamInfo &info) {
  const auto model = info.GetModelName();
  const auto camId = info.GetId();
  const auto serial = info.GetSerialNumber();
  const auto tlType = info.GetTLType();
  const auto vendor = info.GetVendorName();
  const auto usb3VisionGuid = info.GetUSB3VisionGUID();
  const auto usbPortId = info.GetUSBPortID();
  const auto gevIpAddress = info.GetGevIpAddress();
  const auto gevSubnetMask = info.GetGevSubnetMask();
  const auto gevGateway = info.GetGevGateway();
  const auto gevMacAddress = info.GetGevMACAddress();

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

NeoAPI::CamInfoList &discoverCameras() {
  IRSOL_LOG_TRACE("Discovering cameras");
  NeoAPI::CamInfoList &infoList = NeoAPI::CamInfoList::Get();
  IRSOL_LOG_TRACE("Refreshing camera list");
  infoList.Refresh();

  return infoList;
}
namespace internal {
constexpr const char *defaultCameraSerialNumber() { return "700011810487"; }
} // namespace internal

} // namespace utils
} // namespace irsol