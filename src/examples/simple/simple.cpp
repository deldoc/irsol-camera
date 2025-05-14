#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"

int main() {

  irsol::init_logging("log/simple.log");

  IRSOL_LOG_INFO("Starting simple example");
  NeoAPI::Cam cam = NeoAPI::Cam();
  IRSOL_LOG_DEBUG("Created camera object");
  try {
    IRSOL_LOG_DEBUG("Connecting to camera");
    cam.Connect();
    IRSOL_LOG_DEBUG("Connection successful");
  } catch (NeoAPI::NotConnectedException &e) {
    IRSOL_LOG_CRITICAL("Failed to connect to camera: {0}", e.GetDescription());
    return -1;
  }
  IRSOL_LOG_DEBUG("Discovering cameras");
  const auto &discovery = irsol::utils::discover_cameras();
  IRSOL_LOG_DEBUG("Number of cameras found: {0:d}", discovery.size());

  IRSOL_LOG_INFO("Connected to camera");
  return 0;
}