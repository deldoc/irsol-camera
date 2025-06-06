#include "irsol/camera/pixel_format.hpp"

#include <catch2/catch_all.hpp>
#include <vector>

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

TEST_CASE("PixelByteSwapper<true>-swaps-bytes-for-16-bit-data", "[PixelFormat]")
{

  SECTION("Empty range does nothing")
  {
    std::vector<irsol::types::byte_t> v;
    irsol::camera::PixelByteSwapper<true>()(v.begin(), v.end());
    CHECK(v.empty());
  }

  SECTION("Begin after end raises an assertion")
  {
    std::vector<irsol::types::byte_t> v = {irsol::types::byte_t{0x01}, irsol::types::byte_t{0x02}};
    CHECK_THROWS_AS(
      irsol::camera::PixelByteSwapper<true>()(v.end(), v.begin()), irsol::AssertionException);
  }

  SECTION("Even number of bytes swaps pairs")
  {
    std::vector<irsol::types::byte_t> v = {irsol::types::byte_t{0x01},
                                           irsol::types::byte_t{0x02},
                                           irsol::types::byte_t{0x03},
                                           irsol::types::byte_t{0x04}};
    irsol::camera::PixelByteSwapper<true>()(v.begin(), v.end());
    CHECK(
      v == std::vector<irsol::types::byte_t>{irsol::types::byte_t{0x02},
                                             irsol::types::byte_t{0x01},
                                             irsol::types::byte_t{0x04},
                                             irsol::types::byte_t{0x03}});
  }

  SECTION("Odd number of bytes swaps pairs, last byte untouched")
  {
    std::vector<irsol::types::byte_t> v = {
      irsol::types::byte_t{0x01}, irsol::types::byte_t{0x02}, irsol::types::byte_t{0x03}};
    irsol::camera::PixelByteSwapper<true>()(v.begin(), v.end());
    CHECK(
      v == std::vector<irsol::types::byte_t>{
             irsol::types::byte_t{0x02}, irsol::types::byte_t{0x01}, irsol::types::byte_t{0x03}});
  }

  SECTION("Single byte does nothing")
  {
    std::vector<irsol::types::byte_t> v = {irsol::types::byte_t{0xAA}};
    irsol::camera::PixelByteSwapper<true>()(v.begin(), v.end());
    CHECK(v == std::vector<irsol::types::byte_t>{irsol::types::byte_t{0xAA}});
  }

  SECTION("Longer vector swaps all pairs correctly")
  {
    std::vector<irsol::types::byte_t> v = {irsol::types::byte_t{0x01},
                                           irsol::types::byte_t{0x02},
                                           irsol::types::byte_t{0x03},
                                           irsol::types::byte_t{0x04},
                                           irsol::types::byte_t{0x05},
                                           irsol::types::byte_t{0x06},
                                           irsol::types::byte_t{0x07},
                                           irsol::types::byte_t{0x08},
                                           irsol::types::byte_t{0x09},
                                           irsol::types::byte_t{0x0A},
                                           irsol::types::byte_t{0x0B},
                                           irsol::types::byte_t{0x0C}};
    irsol::camera::PixelByteSwapper<true>()(v.begin(), v.end());
    CHECK(
      v == std::vector<irsol::types::byte_t>{irsol::types::byte_t{0x02},
                                             irsol::types::byte_t{0x01},
                                             irsol::types::byte_t{0x04},
                                             irsol::types::byte_t{0x03},
                                             irsol::types::byte_t{0x06},
                                             irsol::types::byte_t{0x05},
                                             irsol::types::byte_t{0x08},
                                             irsol::types::byte_t{0x07},
                                             irsol::types::byte_t{0x0A},
                                             irsol::types::byte_t{0x09},
                                             irsol::types::byte_t{0x0C},
                                             irsol::types::byte_t{0x0B}});
  }
}

TEST_CASE("PixelByteSwapper<false>-is-no-op", "[PixelFormat]")
{

  SECTION("No-op on empty vector")
  {
    std::vector<irsol::types::byte_t> v;
    irsol::camera::PixelByteSwapper<false>()(v.begin(), v.end());
    CHECK(v.empty());
  }

  SECTION("No-op on non-empty vector")
  {
    std::vector<irsol::types::byte_t> v = {irsol::types::byte_t{0x01},
                                           irsol::types::byte_t{0x02},
                                           irsol::types::byte_t{0x03},
                                           irsol::types::byte_t{0x04}};
    irsol::camera::PixelByteSwapper<false>()(v.begin(), v.end());
    CHECK(
      v == std::vector<irsol::types::byte_t>{irsol::types::byte_t{0x01},
                                             irsol::types::byte_t{0x02},
                                             irsol::types::byte_t{0x03},
                                             irsol::types::byte_t{0x04}});
  }
}
