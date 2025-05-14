#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"
#include "spdlog/spdlog.h"

int main() {

  irsol::init_logging("log/simple.log");

  spdlog::info("Starting simple example");
  NeoAPI::Cam cam = NeoAPI::Cam();
  spdlog::debug("Created camera object");
  try {
    spdlog::debug("Connecting to camera");
    cam.Connect();
    spdlog::debug("Connection successful");
  } catch (NeoAPI::NotConnectedException &e) {
    spdlog::error("Failed to connect to camera: {0}", e.GetDescription());
    return -1;
  }
  spdlog::debug("Discovering cameras");
  const auto &discovery = irsol::utils::discover_cameras();
  spdlog::debug("Number of cameras found: {0:d}", discovery.size());

  spdlog::info("Connected to camera");
  return 0;
}