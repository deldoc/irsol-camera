#pragma once
#ifndef IRSOL_USE_OPENCV
#error "IRSOL_USE_OPENCV is not defined. Please define it to use OpenCV functionality."
#endif
#include <neoapi/neoapi.hpp>
#include <opencv2/opencv.hpp>

namespace irsol {
namespace opencv {

enum class ColorConversionMode
{
  NONE          = 1,
  GRAY_TO_COLOR = 2
};

cv::Mat convertImageToMat(
  const NeoAPI::Image& image,
  ColorConversionMode  mode = ColorConversionMode::NONE);

}  // namespace opencv
}  // namespace irsol