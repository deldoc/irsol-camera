#include "irsol/utils.hpp"
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "neoapi/neoapi.hpp"
#include "tabulate/tabulate.hpp"
#include <thread>
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

  tabulate::Table cam_info;
  cam_info.add_row({"Name", "Value"});
  cam_info.add_row({"Camera Model Name", model.c_str()});
  cam_info.add_row({"Camera ID", cam_id.c_str()});
  cam_info.add_row({"Camera Serial Number", serial.c_str()});
  cam_info.add_row({"Camera Transport Layer Type", tl_type.c_str()});
  cam_info.add_row({"Camera Vendor Name", vendor.c_str()});
  cam_info.add_row({"Camera USB3 Vision GUID", usb3_vision_guid.c_str()});
  cam_info.add_row({"Camera USB Port ID", usb_port_id.c_str()});
  cam_info.add_row({"Camera GEV IP Address", gev_ip_address.c_str()});
  cam_info.add_row({"Camera GEV Subnet Mask", gev_subnet_mask.c_str()});
  cam_info.add_row({"Camera GEV Gateway", gev_gateway.c_str()});
  cam_info.add_row({"Camera GEV MAC Address", gev_mac_address.c_str()});
  cam_info.add_row({"Is connectable", info.IsConnectable() ? "true" : "false"});

  cam_info.column(0).format().font_align(tabulate::FontAlign::right);
  IRSOL_LOG_INFO("\n{0:s}", cam_info.str());
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