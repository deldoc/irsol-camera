#include "irsol/irsol.hpp"
int main() {

  irsol::init_logging("log/simple.log");
  irsol::init_assert_handler();

  IRSOL_LOG_DEBUG("Starting simple example");

  auto cam = irsol::utils::load_default_camera();

  IRSOL_LOG_DEBUG("Camera connection successful");
  irsol::utils::log_camera_info(cam.GetInfo());

  irsol::CameraStatusMonitor monitor{cam};
  monitor.start();

  for (int i = 0; i < 10; ++i) {
    IRSOL_LOG_INFO("Iteration {0:d}", i);
    std::this_thread::sleep_for(std::chrono::milliseconds(233));
  }

  monitor.stop();

  IRSOL_LOG_INFO("Successful execution, shutting down");
  return 0;
}