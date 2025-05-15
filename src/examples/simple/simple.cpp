#include "irsol/irsol.hpp"

int main() {

  irsol::init_logging("log/simple.log");
  irsol::init_assert_handler();

  IRSOL_LOG_DEBUG("Starting simple example");

  auto cam = irsol::utils::load_default_camera();

  IRSOL_LOG_DEBUG("Camera connection successful");
  irsol::utils::log_camera_info(cam.GetInfo());

  IRSOL_LOG_INFO("Successful execution, shutting down");
  return 0;
}