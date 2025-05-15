#pragma once
#ifndef IRSOL_USE_OPENCV
#error "IRSOL_USE_OPENCV is not defined. Please define it to use OpenCV functionality."
#endif
#include "neoapi/neoapi.hpp"
#include <opencv2/opencv.hpp>

namespace irsol {
namespace opencv {
cv::Mat convert_image_to_mat(const NeoAPI::Image &image);
}
} // namespace irsol