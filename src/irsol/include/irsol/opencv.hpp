/**
 * @file irsol/opencv.hpp
 * @brief OpenCV integration utilities for NeoAPI image handling.
 *
 * This header defines functions and types for converting Baumer NeoAPI image objects
 * into OpenCV `cv::Mat` objects. These utilities are only available if OpenCV is detected
 * by CMake at build time.
 */

/**
 * @defgroup opencv_integration OpenCV Integration
 * @brief Utilities for interoperability between NeoAPI and OpenCV.
 *
 * This group contains functions and types that facilitate the conversion of Baumer NeoAPI images
 * to OpenCV `cv::Mat` objects. The group is only available if the OpenCV library is found on the
 * user's computer at compilation time. If OpenCV is not detected, this header and its contents
 * are excluded from the build.
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
 * @ingroup opencv_integration
 * @brief Provides utilities for interoperability between NeoAPI and OpenCV.
 *
 * This namespace is only included and compiled if the OpenCV library is found on the user's
 * computer at compilation time. It includes functions and types to convert Baumer NeoAPI images
 * into OpenCV `cv::Mat` objects.
 */
namespace opencv {

/**
 * @ingroup opencv_integration
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

/**
 * @ingroup opencv_integration
 * @brief Creates an OpenCV `cv::Mat` from a raw buffer received from the IRSOL server.
 *
 * This function takes a pointer to a buffer containing image data in unpacked Mono12 format
 * (2 bytes per pixel, little endian, with bytes for each pixel swapped as sent by the server),
 * and constructs a `cv::Mat` of type CV_16UC1. The function swaps the bytes for each pixel
 * to match OpenCV's expected order, copies the data into the matrix, and rescales the pixel
 * values from 12-bit depth ([0, 4095]) to 16-bit depth ([0, 65535]).
 *
 * @param data Pointer to the raw image buffer. The buffer must contain at least `rows * cols * 2`
 * bytes. The data is modified in-place to swap bytes for each pixel.
 * @param rows Number of image rows (height).
 * @param cols Number of image columns (width).
 * @return A `cv::Mat` of type CV_16UC1 containing the image data with pixel values rescaled to 16
 * bits.
 *
 * @note The input buffer is modified in-place due to byte swapping.
 * @note The function assumes the buffer is in unpacked Mono12 format as sent by the IRSOL server.
 * @see irsol::protocol::Serializer::serializeImageBinaryData
 * @see irsol::camera::PixelByteSwapper
 */
cv::Mat createCvMatFromIrsolServerBuffer(unsigned char* data, size_t rows, size_t cols);

}  // namespace opencv
}  // namespace irsol
