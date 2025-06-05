/**
 * @file irsol/opencv.hpp
 * @brief OpenCV integration utilities for NeoAPI image handling.
 *
 * This header defines functions and types for converting Baumer NeoAPI image objects
 * into OpenCV `cv::Mat` objects. These utilities are only available if OpenCV is detected
 * by CMake at build time.
 */
#pragma once
#ifndef IRSOL_USE_OPENCV
#error "IRSOL_USE_OPENCV is not defined. Please define it to use OpenCV functionality."
#endif

#include <neoapi/neoapi.hpp>
#include <opencv2/opencv.hpp>

namespace irsol {

/**
 * @namespace opencv
 * @brief Provides utilities for interoperability between NeoAPI and OpenCV.
 *
 * This namespace includes functions and types to convert Baumer NeoAPI images into
 * OpenCV `cv::Mat` objects. It is only available if OpenCV is found during the build process.
 * Ensure the macro `IRSOL_USE_OPENCV` is defined in your compilation unit to use this
 * functionality.
 */
namespace opencv {

/**
 * @brief Converts a `NeoAPI::Image` (Mono12, non-packed) into an OpenCV `cv::Mat`.
 *
 * @param image The `NeoAPI::Image` to convert containing 12-bit grayscale pixel data. Must be in
 * unpacked Mono12 format (2 bytes per pixel, little endian).
 * @return A `cv::Mat` of type CV_16UC1 with pixel values in range [0, 65535] (pixel raw values from
 * the NeoAPI::Image 12bit depth are rescaled, so that 0 maps to 0 and 2^12-1 (=4095) maps to 2^16-1
 * (=65535))
 *
 * @note The function assumes the `NeoAPI::Image` is valid and has an appropriate pixel format
 * compatible with OpenCV.
 */
cv::Mat convertNeoImageToCvMat(const NeoAPI::Image& image);

}  // namespace opencv
}  // namespace irsol
