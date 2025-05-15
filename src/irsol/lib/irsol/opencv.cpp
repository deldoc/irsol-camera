#include "irsol/assert.hpp"
#include "neoapi/neoapi.hpp"
#include <opencv2/opencv.hpp>

namespace irsol {
namespace opencv {
cv::Mat convert_image_to_mat(const NeoAPI::Image &image) {
  IRSOL_ASSERT_ERROR(image.GetImageData() != nullptr, "Image data is null");
  IRSOL_ASSERT_ERROR(image.GetWidth() > 0 && image.GetHeight() > 0,
                     "Invalid image dimensions: %lu x %lu", image.GetWidth(), image.GetHeight());

  cv::Mat mat(image.GetHeight(), image.GetWidth(), CV_8UC1);
  std::memcpy(mat.data, image.GetImageData(), image.GetSize());
  return mat;
}
} // namespace opencv
} // namespace irsol