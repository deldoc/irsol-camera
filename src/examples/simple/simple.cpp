#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"
#include "spdlog/spdlog.h"

int main() {

  spdlog::info("Starting simple example");
  NeoAPI::Cam cam = NeoAPI::Cam();
  try {
    cam.Connect();
  } catch (NeoAPI::NotConnectedException &e) {
    spdlog::error("Failed to connect to camera: {0}", e.GetDescription());
    return -1;
  }
  irsol::utils::discover_cameras();
  spdlog::info("Connected to camera");
  return 0;
}