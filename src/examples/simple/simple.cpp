#include "spdlog/spdlog.h"
#include "neoapi/neoapi.hpp"
#include "irsol/utils.hpp"

int main() {

  spdlog::info("Starting simple example");
  NeoAPI::Cam cam = NeoAPI::Cam();
  try {
    cam.Connect();
  } catch (NeoAPI::NotConnectedException &e) {
    spdlog::error("Failed to connect to camera: {0}", e.GetDescription());
    return -1;
  }
  irsol::utils::discover_cameras(cam);
  spdlog::info("Connected to camera");
  return 0;
}