#include "irsol/protocol/message/binary.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

TEST_CASE("BinaryDataBuffer::BinaryDataBuffer()", "[Protocol][Protocol::Message]")
{
  auto size = GENERATE(static_cast<uint32_t>(10), static_cast<uint8_t>(1000));
  {

    std::vector<irsol::protocol::internal::byte_t> data(size);
    irsol::protocol::BinaryDataBuffer              m{std::move(data), {size}};
    STATIC_CHECK(m.dim == 1);
    STATIC_CHECK(m.value_t_bytes == 1);  // one element of type internal::byte_t is 1 byte.
    CHECK(m.numElements == size);
    CHECK(m.numBytes == size);
  }
}

TEST_CASE("ImageBinaryData::ImageBinaryData()", "[Protocol][Protocol::Message]")
{
  auto sizeData = GENERATE(
    std::make_pair(
      static_cast<uint64_t>(160),
      std::array<uint32_t, 2>{static_cast<uint32_t>(10), static_cast<uint32_t>(16)}),
    std::make_pair(
      static_cast<uint64_t>(921600),
      std::array<uint32_t, 2>{static_cast<uint32_t>(1280), static_cast<uint32_t>(720)}));
  {
    std::vector<irsol::protocol::internal::byte_t> data(sizeData.first);
    irsol::protocol::ImageBinaryData               m{std::move(data), sizeData.second};
    STATIC_CHECK(m.dim == 2);
    STATIC_CHECK(m.value_t_bytes == 1);  // one element of type internal::byte_t is 1 byte.
    CHECK(m.numElements == sizeData.first);
    CHECK(m.numBytes == sizeData.first);
  }
}

TEST_CASE("ColorImageBinaryData::ColorImageBinaryData()", "[Protocol][Protocol::Message]")
{
  auto sizeData = GENERATE(
    std::make_pair(
      static_cast<uint64_t>(160),
      std::array<uint32_t, 3>{
        static_cast<uint32_t>(10), static_cast<uint32_t>(16), static_cast<uint32_t>(1)}),
    std::make_pair(
      static_cast<uint64_t>(2764800),
      std::array<uint32_t, 3>{
        static_cast<uint32_t>(1280), static_cast<uint32_t>(720), static_cast<uint32_t>(3)}));
  {
    std::vector<irsol::protocol::internal::byte_t> data(sizeData.first);
    irsol::protocol::ColorImageBinaryData          m{std::move(data), sizeData.second};
    STATIC_CHECK(m.dim == 3);
    STATIC_CHECK(m.value_t_bytes == 1);  // one element of type internal::byte_t is 1 byte.
    CHECK(m.numElements == sizeData.first);
    CHECK(m.numBytes == sizeData.first);
  }
}
