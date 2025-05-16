#include "irsol/opencv.hpp"
#include "irsol/assert.hpp"
#include "neoapi/neoapi.hpp"
#include <opencv2/opencv.hpp>

namespace irsol {
namespace opencv {
cv::Mat convertImageToMat(const NeoAPI::Image &image, ColorConversionMode mode) {
  IRSOL_ASSERT_ERROR(image.GetImageData() != nullptr, "Image data is null");
  IRSOL_ASSERT_ERROR(image.GetWidth() > 0 && image.GetHeight() > 0,
                     "Invalid image dimensions: %lu x %lu", image.GetWidth(), image.GetHeight());

  cv::Mat grayImage(image.GetHeight(), image.GetWidth(), CV_8UC1);
  std::memcpy(grayImage.data, image.GetImageData(), image.GetSize());
  if (mode == ColorConversionMode::NONE) {
    return grayImage;
  }
  cv::Mat bgrImage;
  cv::cvtColor(grayImage, bgrImage, cv::COLOR_GRAY2BGR);
  return bgrImage;
}
} // namespace opencv
} // namespace irsol