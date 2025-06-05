#include "irsol/camera/pixel_format.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("Pixel<T>::max())", "[PixelFormat]")
{
  STATIC_CHECK(irsol::camera::Pixel<8>::max() == 255);
  STATIC_CHECK(irsol::camera::Pixel<12>::max() == 4095);
  STATIC_CHECK(irsol::camera::Pixel<16>::max() == 65535);
}

TEST_CASE("PixelConversion<>::scale<T>())", "[PixelFormat]")
{
  CHECK(irsol::camera::PixelConversion<8, 12>::scale(255) == 4095);
  CHECK(irsol::camera::PixelConversion<8, 16>::scale(255) == 65535);
  CHECK(irsol::camera::PixelConversion<12, 16>::scale(4095) == 65535);
  CHECK(irsol::camera::PixelConversion<12, 8>::scale(4095) == 255);
  CHECK(irsol::camera::PixelConversion<16, 8>::scale(65535) == 255);
  CHECK(irsol::camera::PixelConversion<16, 12>::scale(65535) == 4095);
}
