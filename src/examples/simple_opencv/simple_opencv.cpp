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
  IRSOL_LOG_INFO("\n{}", cam.cameraInfoAsString());

  irsol::camera::StatusMonitor monitor{cam, std::chrono::milliseconds(1000)};
  monitor.start();

  for(int i = 0; i < 500; ++i) {

    IRSOL_LOG_INFO("Iteration {0:d}", i);

    auto camExposure = cam.setExposure(std::chrono::microseconds((1 + i) * 10));

    auto image = cam.captureImage();
    if(image.IsEmpty() || image.GetSize() == 0) {
      IRSOL_LOG_ERROR("Failed to capture image at iteration {0:d}", i);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      continue;
    }
    auto image_ts = image.GetTimestamp();
    auto image_id = image.GetImageID();

    cv::Mat cv_image = irsol::opencv::convertNeoImageToCvMat(image);
    cv::putText(
      cv_image,
      "ExposureTime: " + irsol::utils::durationToString(camExposure),
      {20, 50},
      cv::FONT_HERSHEY_COMPLEX,
      .5,
      {65535, 65535, 65535},
      1,
      cv::LINE_AA);
    cv::putText(
      cv_image,
      "Timestamp: " + std::to_string(image_ts),
      {20, 80},
      cv::FONT_HERSHEY_COMPLEX,
      .5,
      {65535, 65535, 65535},
      1,
      cv::LINE_AA);
    cv::putText(
      cv_image,
      "Id: " + std::to_string(image_id),
      {20, 110},
      cv::FONT_HERSHEY_COMPLEX,
      .5,
      {65535, 65535, 65535},
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