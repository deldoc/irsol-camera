#include "irsol/utils.hpp"
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "neoapi/neoapi.hpp"

namespace irsol {
namespace utils {

NeoAPI::Cam load_default_camera() {
  IRSOL_LOG_DEBUG("Loading default camera");
  NeoAPI::Cam cam = NeoAPI::Cam();

  const auto camera_serial_number = internal::default_camera_serial_number();
  IRSOL_LOG_TRACE("Trying to connect to default camera with SN '{0:s}'.", camera_serial_number);
  try {
    cam.Connect(camera_serial_number);
  } catch (NeoAPI::NotConnectedException &e) {
    IRSOL_ASSERT_FATAL(false, "Camera connection failed: %s", e.GetDescription());
    throw e;
  }
  IRSOL_ASSERT_FATAL(cam.IsConnected(), "Camera is not connected");
  IRSOL_LOG_DEBUG("Camera connection successful");
  return cam;
}

void log_camera_info(const NeoAPI::CamInfo &info) {
  const auto model = info.GetModelName();
  const auto cam_id = info.GetId();
  const auto serial = info.GetSerialNumber();
  const auto tl_type = info.GetTLType();
  const auto vendor = info.GetVendorName();
  const auto usb3_vision_guid = info.GetUSB3VisionGUID();
  const auto usb_port_id = info.GetUSBPortID();
  const auto gev_ip_address = info.GetGevIpAddress();
  const auto gev_subnet_mask = info.GetGevSubnetMask();
  const auto gev_gateway = info.GetGevGateway();
  const auto gev_mac_address = info.GetGevMACAddress();
  IRSOL_LOG_INFO("Camera Model Name: {0:s}", model.c_str());
  IRSOL_LOG_INFO("Camera ID: {0:s}", cam_id.c_str());
  IRSOL_LOG_INFO("Camera Serial Number: {0:s}", serial.c_str());
  IRSOL_LOG_INFO("Camera Transport Layer Type: {0:s}", tl_type.c_str());
  IRSOL_LOG_INFO("Camera Vendor Name: {0:s}", vendor.c_str());
  IRSOL_LOG_INFO("Camera USB3 Vision GUID: {0:s}", usb3_vision_guid.c_str());
  IRSOL_LOG_INFO("Camera USB Port ID: {0:s}", usb_port_id.c_str());
  IRSOL_LOG_INFO("Camera GEV IP Address: {0:s}", gev_ip_address.c_str());
  IRSOL_LOG_INFO("Camera GEV Subnet Mask: {0:s}", gev_subnet_mask.c_str());
  IRSOL_LOG_INFO("Camera GEV Gateway: {0:s}", gev_gateway.c_str());
  IRSOL_LOG_INFO("Camera GEV MAC Address: {0:s}", gev_mac_address.c_str());
  IRSOL_LOG_INFO("Is connectable: {0:s}", info.IsConnectable() ? "true" : "false");
}

NeoAPI::CamInfoList &discover_cameras() {
  IRSOL_LOG_TRACE("Discovering cameras");
  NeoAPI::CamInfoList &infolist = NeoAPI::CamInfoList::Get();
  IRSOL_LOG_TRACE("Refreshing camera list");
  infolist.Refresh();

  return infolist;
}
namespace internal {
constexpr const char *default_camera_serial_number() { return "700011810487"; }
} // namespace internal

} // namespace utils
} // namespace irsol