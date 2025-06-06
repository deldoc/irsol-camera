#include "irsol/protocol/message/binary.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

TEST_CASE("BinaryDataBuffer::BinaryDataBuffer()", "[Protocol][Protocol::Message]")
{
  auto size = GENERATE(static_cast<uint64_t>(10), static_cast<uint64_t>(1000));
  {

    std::vector<irsol::types::byte_t> data(
      size * irsol::protocol::BinaryDataBuffer::BYTES_PER_ELEMENT);
    irsol::protocol::BinaryDataBuffer m{std::move(data), {size}};
    STATIC_CHECK(m.DIM == 1);
    STATIC_CHECK(m.BYTES_PER_ELEMENT == 2);  // We work with image data with 16bits depth always.
    CHECK(m.numElements == size);
    CHECK(m.numBytes == size * irsol::protocol::BinaryDataBuffer::BYTES_PER_ELEMENT);
  }
}

TEST_CASE("ImageBinaryData::ImageBinaryData()", "[Protocol][Protocol::Message]")
{
  auto sizeData = GENERATE(
    std::make_pair(
      static_cast<uint64_t>(160),
      std::array<uint64_t, 2>{static_cast<uint64_t>(10), static_cast<uint64_t>(16)}),
    std::make_pair(
      static_cast<uint64_t>(921600),
      std::array<uint64_t, 2>{static_cast<uint64_t>(1280), static_cast<uint64_t>(720)}));
  {
    std::vector<irsol::types::byte_t> data(
      sizeData.first * irsol::protocol::ImageBinaryData::BYTES_PER_ELEMENT);
    irsol::protocol::ImageBinaryData m{std::move(data), sizeData.second};
    STATIC_CHECK(m.DIM == 2);
    STATIC_CHECK(m.BYTES_PER_ELEMENT == 2);  // We work with image data with 16bits depth always.
    CHECK(m.numElements == sizeData.first);
    CHECK(m.numBytes == sizeData.first * irsol::protocol::BinaryDataBuffer::BYTES_PER_ELEMENT);
  }
}

TEST_CASE("ColorImageBinaryData::ColorImageBinaryData()", "[Protocol][Protocol::Message]")
{
  auto sizeData = GENERATE(
    std::make_pair(
      static_cast<uint64_t>(160),
      std::array<uint64_t, 3>{
        static_cast<uint64_t>(10), static_cast<uint64_t>(16), static_cast<uint64_t>(1)}),
    std::make_pair(
      static_cast<uint64_t>(2764800),
      std::array<uint64_t, 3>{
        static_cast<uint64_t>(1280), static_cast<uint64_t>(720), static_cast<uint64_t>(3)}));
  {
    std::vector<irsol::types::byte_t> data(
      sizeData.first * irsol::protocol::ColorImageBinaryData::BYTES_PER_ELEMENT);
    irsol::protocol::ColorImageBinaryData m{std::move(data), sizeData.second};
    STATIC_CHECK(m.DIM == 3);
    STATIC_CHECK(m.BYTES_PER_ELEMENT == 2);  // We work with image data with 16bits depth always.
    CHECK(m.numElements == sizeData.first);
    CHECK(m.numBytes == sizeData.first * irsol::protocol::BinaryDataBuffer::BYTES_PER_ELEMENT);
  }
}
