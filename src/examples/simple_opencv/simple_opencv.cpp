#include "irsol/irsol.hpp"
#include <opencv2/opencv.hpp>

int main() {

  irsol::initLogging("log/simple_opencv.log");
  irsol::initAssertHandler();

  IRSOL_LOG_DEBUG("Starting simple example with OpenCV");

  irsol::CameraInterface cam;

  IRSOL_LOG_DEBUG("Camera connection successful");
  irsol::utils::logCameraInfo(cam.getNeoCam().GetInfo());

  irsol::CameraStatusMonitor monitor{cam, std::chrono::milliseconds(200)};
  monitor.start();

  for (int i = 0; i < 50; ++i) {
    IRSOL_LOG_INFO("Iteration {0:d}", i);
    auto image = cam.captureImage();
    auto image_size = image.GetSize();
    IRSOL_LOG_INFO("Image size: {0:d}", image_size);
    auto image_ts = image.GetTimestamp();

    double current_exposure = cam.getParam<float>("ExposureTime");

    cv::Mat cv_image =
        irsol::opencv::convertImageToMat(image, irsol::opencv::ColorConversionMode::GRAY_TO_COLOR);
    cv::putText(cv_image, "Exposure: " + std::to_string(current_exposure), {20, 50},
                cv::FONT_HERSHEY_COMPLEX, 1.5, {0, 0, 255}, 1, cv::LINE_AA);
    cv::putText(cv_image, "Timestamp: " + std::to_string(image_ts), {20, 80},
                cv::FONT_HERSHEY_COMPLEX, 1, {0, 255, 0}, 1, cv::LINE_AA);
    cv::imshow("image", cv_image);

    bool closeWindow = false;
    const int keyPressed = cv::waitKey(1) & 0xFF;
    switch (keyPressed) {
    case 27: { // Esc
      IRSOL_LOG_INFO("Closing window request accepted");
      closeWindow = true;
      break;
    }
    case 113: { // q
      IRSOL_LOG_INFO("Closing window request accepted");
      closeWindow = true;
      break;
    }
    }

    if (closeWindow) {
      break;
    }

    uint64_t newExposureTime = i * 100;
    IRSOL_LOG_DEBUG("Setting exposure time to {0:d}ms", newExposureTime);
    cam.setParam("ExposureTime", newExposureTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  cv::destroyAllWindows();

  monitor.stop();

  IRSOL_LOG_INFO("Successful execution, shutting down");
  return 0;
}