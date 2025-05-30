#include "irsol/irsol.hpp"

#include <opencv2/opencv.hpp>

int
main()
{

  irsol::initLogging("log/simple_opencv.log");
  irsol::initAssertHandler();

  IRSOL_LOG_DEBUG("Starting simple example with OpenCV");

  irsol::camera::Interface cam = irsol::camera::Interface::HalfResolution();

  IRSOL_LOG_DEBUG("Camera connection successful");
  irsol::utils::logCameraInfo(cam.getNeoCam().GetInfo());

  irsol::camera::StatusMonitor monitor{cam, std::chrono::milliseconds(1000)};
  monitor.start();

  for(int i = 0; i < 500; ++i) {

    IRSOL_LOG_INFO("Iteration {0:d}", i);

    if(i % 10 == 0) {
      auto t0 = irsol::types::clock_t::now();
      cam.setParam("Width", 100 + i);
      auto t1 = irsol::types::clock_t::now();
      cam.setParam("OffsetX", 100 + i);
      auto t2 = irsol::types::clock_t::now();

      IRSOL_LOG_INFO(
        "Set Width in {} ms, Set OffsetX in {} ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count(),
        std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
    }

    auto image = cam.captureImage();
    if(image.IsEmpty() || image.GetSize() == 0) {
      IRSOL_LOG_ERROR("Failed to capture image at iteration {0:d}", i);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      continue;
    }
    auto image_size = image.GetSize();
    IRSOL_LOG_INFO("Image size: {0:d}", image_size);
    auto image_ts = image.GetTimestamp();

    int currentWidth = cam.getParam<int>("Width");

    cv::Mat cv_image =
      irsol::opencv::convertImageToMat(image, irsol::opencv::ColorConversionMode::GRAY_TO_COLOR);
    cv::putText(
      cv_image,
      "Width: " + std::to_string(currentWidth),
      {20, 50},
      cv::FONT_HERSHEY_COMPLEX,
      .5,
      {0, 0, 255},
      1,
      cv::LINE_AA);
    cv::putText(
      cv_image,
      "Timestamp: " + std::to_string(image_ts),
      {20, 80},
      cv::FONT_HERSHEY_COMPLEX,
      .5,
      {0, 255, 0},
      1,
      cv::LINE_AA);
    cv::imshow("image", cv_image);

    bool      closeWindow = false;
    const int keyPressed  = cv::waitKey(1) & 0xFF;
    switch(keyPressed) {
      case 27: {  // Esc
        IRSOL_LOG_INFO("Closing window request accepted");
        closeWindow = true;
        break;
      }
      case 113: {  // q
        IRSOL_LOG_INFO("Closing window request accepted");
        closeWindow = true;
        break;
      }
    }

    if(closeWindow) {
      break;
    }
  }

  cv::destroyAllWindows();

  monitor.stop();

  IRSOL_LOG_INFO("Successful execution, shutting down");
  return 0;
}