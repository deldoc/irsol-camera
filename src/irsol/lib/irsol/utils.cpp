#include "neoapi/neoapi.hpp"
#include "spdlog/spdlog.h"

namespace irsol {
namespace utils {

void log_camera_info(const NeoAPI::CamInfo& info) {
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
  spdlog::info("Camera Model Name: {0:s}", model.c_str());
  spdlog::info("Camera ID: {0:s}", cam_id.c_str());
  spdlog::info("Camera Serial Number: {0:s}", serial.c_str());
  spdlog::info("Camera Transport Layer Type: {0:s}", tl_type.c_str());
  spdlog::info("Camera Vendor Name: {0:s}", vendor.c_str());
  spdlog::info("Camera USB3 Vision GUID: {0:s}", usb3_vision_guid.c_str());
  spdlog::info("Camera USB Port ID: {0:s}", usb_port_id.c_str());
  spdlog::info("Camera GEV IP Address: {0:s}", gev_ip_address.c_str());
  spdlog::info("Camera GEV Subnet Mask: {0:s}", gev_subnet_mask.c_str());
  spdlog::info("Camera GEV Gateway: {0:s}", gev_gateway.c_str());
  spdlog::info("Camera GEV MAC Address: {0:s}", gev_mac_address.c_str());
  spdlog::info("Is connectable: {0:s}", info.IsConnectable() ? "true" : "false");
}

NeoAPI::CamInfoList& discover_cameras() {
  spdlog::debug("Discovering cameras...");
  NeoAPI::CamInfoList &infolist = NeoAPI::CamInfoList::Get();
  infolist.Refresh();
  spdlog::debug("Number of cameras found: {0:d}", infolist.size());

  return infolist;
}
} // namespace utils
} // namespace irsol