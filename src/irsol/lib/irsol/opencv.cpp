#include "irsol/opencv.hpp"

#include "irsol/assert.hpp"
#include "irsol/camera/pixel_format.hpp"

#include <neoapi/neoapi.hpp>
#include <opencv2/opencv.hpp>

namespace irsol {
namespace opencv {
cv::Mat
convertNeoImageToCvMat(const NeoAPI::Image& image)
{
  const size_t width     = image.GetWidth();
  const size_t height    = image.GetHeight();
  const size_t numPixels = width * height;

  IRSOL_ASSERT_ERROR(
    image.GetPixelFormat() == NeoAPI::NeoString("Mono12"),
    "Only 'Mono12' pixel format are supported, got '%s'",
    image.GetPixelFormat().c_str());

  IRSOL_ASSERT_ERROR(image.GetImageData() != nullptr, "Image data is null");
  IRSOL_ASSERT_ERROR(
    image.GetSize() == numPixels * 2, "Invalid image size for Mono12 unpacked format");

  // Raw image data as bytes
  const uint8_t* rawData = reinterpret_cast<const uint8_t*>(image.GetImageData());

  // Output Mat with 16-bit unsigned pixels
  cv::Mat                                   output(height, width, CV_16UC1);
  irsol::camera::Pixel<16>::representation* outputPtr =
    output.ptr<irsol::camera::Pixel<16>::representation>();
  for(size_t i = 0; i < numPixels; ++i) {
    // Each pixel takes 2 bytes: LSB + MSB (little endian), we make sure to mask the top 4 bits,
    // even if the should already be 0
    irsol::camera::Pixel<16>::representation value =
      (static_cast<irsol::camera::Pixel<16>::representation>(rawData[2 * i]) |
       (static_cast<irsol::camera::Pixel<16>::representation>(rawData[2 * i + 1]) << 8)) &
      0x0FFF;
    // rescale to map [0, 4095] → [0, 65535] from the 12bit depth to the 16bit depth representation
    outputPtr[i] = irsol::camera::PixelConversion<12, 16>::scale(value);
  }

  return output;
}

/**
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
 */
cv::Mat
createCvMatFromIrsolServerBuffer(unsigned char* data, size_t rows, size_t cols)
{
  // Create Mat from buffer
  cv::Mat mat(rows, cols, CV_16UC1);
  size_t  expectedSize = rows * cols * 2;  // 2 bytes per pixel for Mono12 unpacked

  // Change the order of the bytes as the server sends the data where bytes for a single pixel are
  // swapped and OpenCV expects them in the correct order.
  irsol::camera::PixelByteSwapper<true>()(data, data + expectedSize);

  // Copy the data into the cv::Mat
  memcpy(mat.data, data, expectedSize);

  // Scale the values from the 12-bit depth to 16-bit depth
  mat *= irsol::camera::PixelConversion<12, 16>::factor;
  return mat;
}

}  // namespace opencv
}  // namespace irsol