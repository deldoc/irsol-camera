#include "irsol/protocol/message.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

TEST_CASE("Assignment::Assignment()", "[Protocol][Protocol::Message]")
{
  auto value = GENERATE(
    irsol::protocol::internal::value_t(1),
    irsol::protocol::internal::value_t(3.14),
    irsol::protocol::internal::value_t("hello world"));
  {
    auto identifier = GENERATE(
      "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

    irsol::protocol::Assignment m(identifier, value);
    CHECK(m.identifier == identifier);
    CHECK(m.value == value);
  }
  {
    auto identifier =
      GENERATE("", " ", " x", "2", "4x", "identifier with space", "identifier-with-dash");
    CHECK_THROWS_AS(irsol::protocol::Assignment(identifier, value), std::invalid_argument);
  }
}

TEST_CASE("Assignment<int>::hasInt()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto value      = GENERATE(42, 5, 32121);

  irsol::protocol::Assignment m{identifier, value};
  CHECK(m.hasInt());
  CHECK_FALSE(m.hasDouble());
  CHECK_FALSE(m.hasString());
}

TEST_CASE("Assignment<double>::hasDouble()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto value      = GENERATE(42.0123, 5.0, 32121.9999);

  irsol::protocol::Assignment m{identifier, value};
  CHECK_FALSE(m.hasInt());
  CHECK(m.hasDouble());
  CHECK_FALSE(m.hasString());
}

TEST_CASE("Assignment<string>::hasString()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto value      = GENERATE("c", "longer", "long_string_with_underscores", "5è4?-é");

  irsol::protocol::Assignment m{identifier, std::string(value)};
  CHECK_FALSE(m.hasInt());
  CHECK_FALSE(m.hasDouble());
  CHECK(m.hasString());
}

TEST_CASE("Inquiry::Inquiry()", "[Protocol][Protocol::Message]")
{

  {
    auto identifier = GENERATE(
      "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

    irsol::protocol::Inquiry m(identifier);
    CHECK(m.identifier == identifier);
  }
  {
    auto identifier =
      GENERATE("", " ", " x", "2", "4x", "identifier with space", "identifier-with-dash");
    CHECK_THROWS_AS(irsol::protocol::Inquiry(identifier), std::invalid_argument);
  }
}

TEST_CASE("Command::Command()", "[Protocol][Protocol::Message]")
{

  {
    auto identifier = GENERATE(
      "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

    irsol::protocol::Command m(identifier);
    CHECK(m.identifier == identifier);
  }
  {
    auto identifier =
      GENERATE("", " ", " x", "2", "4x", "identifier with space", "identifier-with-dash");
    CHECK_THROWS_AS(irsol::protocol::Command(identifier), std::invalid_argument);
  }
}

TEST_CASE("getInMessageKind<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    STATIC_CHECK(
      irsol::protocol::getInMessageKind(m) == irsol::protocol::InMessageKind::ASSIGNMENT);
  }
  {
    auto m = irsol::protocol::Inquiry{"x"};
    STATIC_CHECK(irsol::protocol::getInMessageKind(m) == irsol::protocol::InMessageKind::INQUIRY);
  }
  {
    auto m = irsol::protocol::Command{"x"};
    STATIC_CHECK(irsol::protocol::getInMessageKind(m) == irsol::protocol::InMessageKind::COMMAND);
  }
}

TEST_CASE("getInMessageKind<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Assignment{"x", 42});
    CHECK(irsol::protocol::getInMessageKind(m) == irsol::protocol::InMessageKind::ASSIGNMENT);
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Inquiry{"x"});
    CHECK(irsol::protocol::getInMessageKind(m) == irsol::protocol::InMessageKind::INQUIRY);
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Command{"x"});
    CHECK(irsol::protocol::getInMessageKind(m) == irsol::protocol::InMessageKind::COMMAND);
  }
}

TEST_CASE("isAssignment<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    STATIC_CHECK(irsol::protocol::isAssignment(m));
  }
  {
    auto m = irsol::protocol::Inquiry{"x"};
    STATIC_CHECK_FALSE(irsol::protocol::isAssignment(m));
  }
  {
    auto m = irsol::protocol::Command{"x"};
    STATIC_CHECK_FALSE(irsol::protocol::isAssignment(m));
  }
}

TEST_CASE("isAssignment<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Assignment{"x", 42});
    CHECK(irsol::protocol::isAssignment(m));
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Inquiry{"x"});
    CHECK_FALSE(irsol::protocol::isAssignment(m));
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Command{"x"});
    CHECK_FALSE(irsol::protocol::isAssignment(m));
  }
}

TEST_CASE("Status::Status()", "[Protocol][Protocol::Message]")
{
  auto body = GENERATE(std::optional<std::string>("hello world"));
  {
    auto identifier = GENERATE(
      "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

    irsol::protocol::Status m1(identifier, body);
    CHECK(m1.identifier == identifier);
    CHECK(m1.body == body);
    irsol::protocol::Status m2(identifier);
    CHECK(m2.identifier == identifier);
    CHECK(m2.body == std::nullopt);
  }
  {
    auto identifier =
      GENERATE("", " ", " x", "2", "4x", "identifier with space", "identifier-with-dash");
    CHECK_THROWS_AS(irsol::protocol::Status(identifier, body), std::invalid_argument);
    CHECK_THROWS_AS(irsol::protocol::Status(identifier), std::invalid_argument);
  }
}

TEST_CASE("Status::hasBody()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");

  {
    irsol::protocol::Status m{identifier, std::make_optional("body")};
    CHECK(m.hasBody());
  }
  {
    irsol::protocol::Status m{identifier, std::nullopt};
    CHECK_FALSE(m.hasBody());
  }
  {
    irsol::protocol::Status m{identifier};
    CHECK_FALSE(m.hasBody());
  }
}

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

TEST_CASE("Error::Error()", "[Protocol][Protocol::Message]")
{
  auto description = GENERATE("hello world", "error description");
  {
    auto identifier = GENERATE(
      "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

    irsol::protocol::Error m(identifier, description);
    CHECK(m.identifier == identifier);
    CHECK(m.description == description);
  }
  {
    auto identifier =
      GENERATE("", " ", " x", "2", "4x", "identifier with space", "identifier-with-dash");
    CHECK_THROWS_AS(irsol::protocol::Error(identifier, description), std::invalid_argument);
  }
}
TEST_CASE("getOutMessageKind<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Status{"status"};
    STATIC_CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::STATUS);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(10);
    auto m = irsol::protocol::BinaryDataBuffer{std::move(data), {10}};
    STATIC_CHECK(
      irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::BINARY_BUFFER);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(160);
    auto m = irsol::protocol::ImageBinaryData{std::move(data), {10, 16}};
    STATIC_CHECK(
      irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::BW_IMAGE);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(480);
    auto m = irsol::protocol::ColorImageBinaryData{std::move(data), {10, 16, 3}};
    STATIC_CHECK(
      irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::COLOR_IMAGE);
  }
  {
    auto m = irsol::protocol::Error{"error", "description"};
    STATIC_CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::ERROR);
  }
}
TEST_CASE("getOutMessageKind<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::Status{"status"});
    CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::STATUS);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(10);
    auto m = irsol::protocol::OutMessage(irsol::protocol::BinaryDataBuffer{std::move(data), {10}});
    CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::BINARY_BUFFER);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(160);
    auto                                           m =
      irsol::protocol::OutMessage(irsol::protocol::ImageBinaryData{std::move(data), {10, 16}});
    CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::BW_IMAGE);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(480);
    auto                                           m = irsol::protocol::OutMessage(
      irsol::protocol::ColorImageBinaryData{std::move(data), {10, 16, 3}});
    CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::COLOR_IMAGE);
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::Error{"error", "description"});
    CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::ERROR);
  }
}
