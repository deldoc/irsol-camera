#include "irsol/logging.hpp"
#include "irsol/utils.hpp"
#include "neoapi/neoapi.hpp"
#include "spdlog/spdlog.h"
#include <opencv2/opencv.hpp>
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

  uint64_t frame_count = 0;
  while (true) {
    NeoAPI::Image image = cam.GetImage();
    spdlog::info("Image captured");

    auto image_data = image.GetImageData();
    if (image_data == nullptr) {
      spdlog::error("Image data is null");
      return -1;
    }
    cv::Mat mat(image.GetHeight(), image.GetWidth(), CV_8UC1);
    // fill the image data into the cv::Mat
    std::memcpy(mat.data, image_data, image.GetSize());

    // Resize the image by 50%
    cv::resize(mat, mat, cv::Size(), 0.5, 0.5);

    auto image_id = image.GetImageID();
    frame_count++;

    auto diff = image_id - frame_count;
    spdlog::info("Image ID: {0:d}, Frame Count: {1:d}, Difference: {2:d}", image_id, frame_count,
                 diff);
    cv::putText(mat, "image-id: " + std::to_string(image_id), cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::putText(mat, "frame-count: " + std::to_string(frame_count), cv::Point(10, 50),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::imshow("image", mat);
    auto ret = cv::waitKey(1);
    if (ret == 27) { // ESC key
      spdlog::info("ESC key pressed, exiting");
      break;
    }
  }
  spdlog::info("Disconnecting from camera");
  cam.Disconnect();
  spdlog::info("Disconnected from camera");
  spdlog::info("Exiting simple example");
  return 0;
}