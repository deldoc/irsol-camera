#include "irsol/irsol.hpp"
#include <opencv2/opencv.hpp>

void frame_display(NeoAPI::Cam &cam) {
  uint64_t frame_count = 0;
  while (true) {
    NeoAPI::Image image = cam.GetImage();
    IRSOL_LOG_DEBUG("Image number {0} captured", frame_count);

    auto cv_image = irsol::opencv::convert_image_to_mat(image);
    if (cv_image.empty()) {
      IRSOL_LOG_ERROR("Image {0} is empty", frame_count);
      continue;
    }

    // Resize the image by 50%
    cv::resize(cv_image, cv_image, cv::Size(), 0.5, 0.5);

    auto image_id = image.GetImageID();
    frame_count++;

    auto diff = image_id - frame_count;
    IRSOL_LOG_INFO("Image ID: {0:d}, Frame Count: {1:d}, Difference: {2:d}", image_id, frame_count,
                   diff);
    cv::putText(cv_image, "image-id: " + std::to_string(image_id), cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::putText(cv_image, "frame-count: " + std::to_string(frame_count), cv::Point(10, 50),
                cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 2);
    cv::imshow("image", cv_image);
    auto ret = cv::waitKey(1);
    if (ret == 27) { // ESC key
      IRSOL_LOG_INFO("ESC key pressed, exiting");
      break;
    }
  }
}

int main() {

  irsol::init_logging("log/simple_opencv.log");
  irsol::init_assert_handler();

  IRSOL_LOG_DEBUG("Starting simple example with OpenCV");

  auto cam = irsol::utils::load_default_camera();

  frame_display(cam);
  IRSOL_LOG_INFO("Successful execution, shutting down.");
  return 0;
}