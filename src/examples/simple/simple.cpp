#include "irsol/irsol.hpp"
#include <chrono>
int main() {

  irsol::init_logging("log/simple.log");
  irsol::init_assert_handler();

  IRSOL_LOG_DEBUG("Starting simple example");

  auto cam = irsol::utils::load_default_camera();

  IRSOL_LOG_DEBUG("Camera connection successful");
  irsol::utils::log_camera_info(cam.GetInfo());

  irsol::CameraStatusMonitor monitor{cam, std::chrono::milliseconds(200)};
  monitor.start();

  for (int i = 0; i < 50; ++i) {
    IRSOL_LOG_INFO("Iteration {0:d}", i);
    cam.GetImage();
    uint64_t newExposureTime = i * 100;
    IRSOL_LOG_INFO("Setting exposure time to {0:d}ms", newExposureTime);
    cam.f().ExposureTime = newExposureTime;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  monitor.stop();

  IRSOL_LOG_INFO("Successful execution, shutting down");
  return 0;
}