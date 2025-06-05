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
}  // namespace opencv
}  // namespace irsol