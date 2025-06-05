/// @file
/// @brief Demonstrates how to get and set camera features using `irsol::camera::Interface`.

#include "irsol/irsol.hpp"

/// @brief Returns the program name for logging.
const std::string
getProgramName()
{
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "camera-feature-demo"
#endif
  return PROGRAM_NAME;
}

/// @brief Initializes camera and logs various feature values.
void
demoCameraFeatures(irsol::camera::Interface& cam)
{
  IRSOL_LOG_INFO("=== Original Camera Information ===");
  IRSOL_LOG_INFO("\n{}", cam.cameraStatusAsString());

  IRSOL_LOG_INFO("=== Reading Common Camera Parameters ===");
  auto mode = cam.getParam<std::string>("AcquisitionMode");
  IRSOL_LOG_INFO("AcquisitionMode: {}", mode);

  auto fpsEnable = cam.getParam<bool>("AcquisitionFrameRateEnable");
  auto fps       = cam.getParam<double>("AcquisitionFrameRate");
  IRSOL_LOG_INFO("AcquisitionFrameRateEnable: {}", fpsEnable);
  IRSOL_LOG_INFO("AcquisitionFrameRate: {:.2f}", fps);

  auto exposure = cam.getExposure();
  IRSOL_LOG_INFO("Current exposure time: {}", irsol::utils::durationToString(exposure));

  IRSOL_LOG_INFO("=== Modifying Parameters ===");
  // Set exposure
  auto newExposure       = std::chrono::microseconds(15000);
  auto actualNewExposure = cam.setExposure(newExposure);
  IRSOL_LOG_INFO(
    "New exposure set to: {}, actual value now: {}",
    irsol::utils::durationToString(newExposure),
    irsol::utils::durationToString(actualNewExposure));

  // Set FPS (requires enabling first)
  auto newAcquisitionFrameRateEnable = true;
  auto actualAcquisitionFrameRateEnable =
    cam.setParam("AcquisitionFrameRateEnable", newAcquisitionFrameRateEnable);
  IRSOL_LOG_INFO(
    "New acquisition frame rate enablement set to: {}, actual value now: {}",
    newAcquisitionFrameRateEnable,
    actualAcquisitionFrameRateEnable);
  auto newAcquisitionFrameRate    = 12.5;
  auto actualAcquisitionFrameRate = cam.setParam("AcquisitionFrameRate", newAcquisitionFrameRate);
  IRSOL_LOG_INFO(
    "New acquisition frame rate set to: {}, actual value now: {}",
    newAcquisitionFrameRate,
    actualAcquisitionFrameRate);

  // Change acquisition mode to SingleFrame
  cam.setParam("AcquisitionMode", "SingleFrame");
  IRSOL_LOG_INFO("AcquisitionMode set to SingleFrame");

  // Use setMultiParam for grouped settings
  cam.setMultiParam({
    {"OffsetX", 0},
    {"OffsetY", 0},
    {"Height", 124},
  });
  IRSOL_LOG_INFO("Updated Offset and Height window");

  IRSOL_LOG_INFO("=== Capturing Image ===");
  auto image = cam.captureImage();
  if(image.IsEmpty()) {
    IRSOL_LOG_WARN("Image is empty.");
  } else {
    IRSOL_LOG_INFO(
      "Captured image: {}x{} ({} bytes)", image.GetHeight(), image.GetWidth(), image.GetSize());
  }

  IRSOL_LOG_INFO("=== Reading Back New Parameter Values ===");
  IRSOL_LOG_INFO("AcquisitionFrameRate: {:.2f}", cam.getParam<double>("AcquisitionFrameRate"));
  IRSOL_LOG_INFO("ExposureTime: {} ", irsol::utils::durationToString(cam.getExposure()));
  IRSOL_LOG_INFO("AcquisitionMode: {}", cam.getParam<std::string>("AcquisitionMode"));

  IRSOL_LOG_INFO("=== Final Camera Information ===");
  IRSOL_LOG_INFO("\n{}", cam.cameraStatusAsString());
}

/// @brief Main entry point for the feature demo application.
int
main()
{
  // Initialize logging
  std::string logPath = "logs/" + getProgramName() + ".log";
  irsol::initLogging(logPath.c_str(), spdlog::level::debug);
  irsol::initAssertHandler();

  // Create camera interface
  auto cam = irsol::camera::Interface::HalfResolution();

  IRSOL_ASSERT_FATAL(cam.isConnected(), "Camera is not connected");

  // Run feature demo
  demoCameraFeatures(cam);

  return 0;
}
