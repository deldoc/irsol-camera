/** \example getting_started.cpp
    This example describes the FIRST STEPS of handling Cam SDK.
    The given source code applies to handle one camera and image acquisition
    Copyright (c) by Baumer Optronic GmbH. All rights reserved, please see the
   provided license for full details.
*/

#include "neoapi/neoapi.hpp"
#include "spdlog/spdlog.h"
#include <chrono>
#include <iostream>
#include <thread>

void run_example() {

  NeoAPI::CamInfoList &infolist = NeoAPI::CamInfoList::Get();
  infolist.Refresh();
  spdlog::info("Number of cameras found: {0:d}", infolist.size());

  for (NeoAPI::CamInfo info : infolist) {
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
  const char *camera_serial_number = "700011810487";
  spdlog::info("Connecting to camera with serial number: {0:s}", camera_serial_number);
  NeoAPI::Cam camera = NeoAPI::Cam();
  camera.Connect(camera_serial_number);
  if (!camera.IsConnected()) {
    throw NeoAPI::NotConnectedException("Camera not connected");
  }
  camera.f().ExposureTime.Set(10000);

  for (int i = 0; i < 10; i++) {
    NeoAPI::Image image = camera.GetImage();
    spdlog::debug("Writing image {0:d}", i);
    const NeoAPI::NeoString image_name =
        NeoAPI::NeoString(("getting_started_" + std::to_string(i) + ".bmp").c_str());
    image.Save(image_name);
    spdlog::info("Image {0:s} saved", image_name.c_str());
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
int main() {
  int result = 0;
  try {
    run_example();
  } catch (NeoAPI::NotConnectedException &exc) {
    spdlog::error("Camera not connected: {0:s}", exc.GetDescription());
    result = 1;
  } catch (NeoAPI::NoAccessException &exc) {
    spdlog::error("Camera is not longer accessible: {0:s}", exc.GetDescription());
    result = 1;
  } catch (NeoAPI::NeoException &exc) {
    spdlog::error("Unknown Camera error:  {0:s}", exc.GetDescription());
    result = 1;
  } catch (...) {
    spdlog::error("Unknown error: unknown exception");
    result = 1;
  }

  return result;
}
