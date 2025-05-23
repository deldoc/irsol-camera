#include "irsol/protocol/message/assignment.hpp"
#include "irsol/protocol/message/binary.hpp"
#include "irsol/protocol/message/command.hpp"
#include "irsol/protocol/message/error.hpp"
#include "irsol/protocol/message/inquiry.hpp"
#include "irsol/protocol/message/success.hpp"
#include "irsol/protocol/message/variants.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>
#include <utility>

TEST_CASE("InMessageKindToString()", "[Protocol][Protocol::Message]")
{
  auto [messageKind, expected] = GENERATE(
    std::make_pair(irsol::protocol::InMessageKind::ASSIGNMENT, "ASSIGNMENT"),
    std::make_pair(irsol::protocol::InMessageKind::INQUIRY, "INQUIRY"),
    std::make_pair(irsol::protocol::InMessageKind::COMMAND, "COMMAND"));
  CHECK(irsol::protocol::InMessageKindToString(messageKind) == expected);
}

TEST_CASE("OutMessageKindToString()", "[Protocol][Protocol::Message]")
{
  auto [messageKind, expected] = GENERATE(
    std::make_pair(irsol::protocol::OutMessageKind::STATUS, "STATUS"),
    std::make_pair(irsol::protocol::OutMessageKind::BINARY_BUFFER, "BINARY_BUFFER"),
    std::make_pair(irsol::protocol::OutMessageKind::BW_IMAGE, "BW_IMAGE"),
    std::make_pair(irsol::protocol::OutMessageKind::COLOR_IMAGE, "COLOR_IMAGE"),
    std::make_pair(irsol::protocol::OutMessageKind::ERROR, "ERROR"));
  CHECK(irsol::protocol::OutMessageKindToString(messageKind) == expected);
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

TEST_CASE("isInquiry<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Inquiry{"x"};
    STATIC_CHECK(irsol::protocol::isInquiry(m));
  }
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    STATIC_CHECK_FALSE(irsol::protocol::isInquiry(m));
  }
  {
    auto m = irsol::protocol::Command{"x"};
    STATIC_CHECK_FALSE(irsol::protocol::isInquiry(m));
  }
}

TEST_CASE("isInquiry<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Inquiry{"x"});
    CHECK(irsol::protocol::isInquiry(m));
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Assignment{"x", 42});
    CHECK_FALSE(irsol::protocol::isInquiry(m));
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Command{"x"});
    CHECK_FALSE(irsol::protocol::isInquiry(m));
  }
}

TEST_CASE("isCommand<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Command{"x"};
    STATIC_CHECK(irsol::protocol::isCommand(m));
  }
  {
    auto m = irsol::protocol::Inquiry{"x"};
    STATIC_CHECK_FALSE(irsol::protocol::isCommand(m));
  }
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    STATIC_CHECK_FALSE(irsol::protocol::isCommand(m));
  }
}

TEST_CASE("isCommand<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Command{"x"});
    CHECK(irsol::protocol::isCommand(m));
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Inquiry{"x"});
    CHECK_FALSE(irsol::protocol::isCommand(m));
  }
  {
    auto m = irsol::protocol::InMessage(irsol::protocol::Assignment{"x", 42});
    CHECK_FALSE(irsol::protocol::isCommand(m));
  }
}
TEST_CASE("getOutMessageKind<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Success::from(irsol::protocol::Command{"x"});
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
    auto m = irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message");
    STATIC_CHECK(irsol::protocol::getOutMessageKind(m) == irsol::protocol::OutMessageKind::ERROR);
  }
}
TEST_CASE("getOutMessageKind<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m        = irsol::protocol::Success::from(irsol::protocol::Command{"x"});
    auto mVariant = irsol::protocol::OutMessage(std::move(m));
    CHECK(irsol::protocol::getOutMessageKind(mVariant) == irsol::protocol::OutMessageKind::STATUS);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(10);
    auto m        = irsol::protocol::BinaryDataBuffer{std::move(data), {10}};
    auto mVariant = irsol::protocol::OutMessage{std::move(m)};
    CHECK(
      irsol::protocol::getOutMessageKind(mVariant) ==
      irsol::protocol::OutMessageKind::BINARY_BUFFER);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(160);
    auto m        = irsol::protocol::ImageBinaryData{std::move(data), {10, 16}};
    auto mVariant = irsol::protocol::OutMessage{std::move(m)};
    CHECK(
      irsol::protocol::getOutMessageKind(mVariant) == irsol::protocol::OutMessageKind::BW_IMAGE);
  }
  {
    std::vector<irsol::protocol::internal::byte_t> data(480);
    auto m        = irsol::protocol::ColorImageBinaryData{std::move(data), {10, 16, 3}};
    auto mVariant = irsol::protocol::OutMessage{std::move(m)};
    CHECK(
      irsol::protocol::getOutMessageKind(mVariant) == irsol::protocol::OutMessageKind::COLOR_IMAGE);
  }
  {
    auto m        = irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message");
    auto mVariant = irsol::protocol::OutMessage(std::move(m));
    CHECK(irsol::protocol::getOutMessageKind(mVariant) == irsol::protocol::OutMessageKind::ERROR);
  }
}

TEST_CASE("isSuccess<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Success::from(irsol::protocol::Command{"x"});
    STATIC_CHECK(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::BinaryDataBuffer{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::ImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::ColorImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message");
    STATIC_CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
}
TEST_CASE("isSuccess<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m =
      irsol::protocol::OutMessage(irsol::protocol::Success::from(irsol::protocol::Command{"x"}));
    CHECK(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::BinaryDataBuffer{{}, {}});
    CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ColorImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
  {
    auto m = irsol::protocol::OutMessage(
      irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message"));
    CHECK_FALSE(irsol::protocol::isSuccess(m));
  }
}

TEST_CASE("isBinaryDataBuffer<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::BinaryDataBuffer{{}, {}};
    STATIC_CHECK(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m = irsol::protocol::Success::from(irsol::protocol::Command{"x"});
    STATIC_CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m = irsol::protocol::ImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m = irsol::protocol::ColorImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m = irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message");
    STATIC_CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
}

TEST_CASE("isBinaryDataBuffer<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::BinaryDataBuffer{{}, {}});
    CHECK(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m =
      irsol::protocol::OutMessage(irsol::protocol::Success::from(irsol::protocol::Command{"x"}));
    CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ColorImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
  {
    auto m = irsol::protocol::OutMessage(
      irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message"));
    CHECK_FALSE(irsol::protocol::isBinaryDataBuffer(m));
  }
}

TEST_CASE("isImageBinaryData<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::ImageBinaryData{{}, {}};
    STATIC_CHECK(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::Success::from(irsol::protocol::Command{"x"});
    STATIC_CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::BinaryDataBuffer{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::ColorImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message");
    STATIC_CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
}

TEST_CASE("isImageBinaryData<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ImageBinaryData{{}, {}});
    CHECK(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m =
      irsol::protocol::OutMessage(irsol::protocol::Success::from(irsol::protocol::Command{"x"}));
    CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::BinaryDataBuffer{{}, {}});
    CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ColorImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::OutMessage(
      irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message"));
    CHECK_FALSE(irsol::protocol::isImageBinaryData(m));
  }
}

TEST_CASE("isColorImageBinaryData<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::ColorImageBinaryData{{}, {}};
    STATIC_CHECK(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::Success::from(irsol::protocol::Command{"x"});
    STATIC_CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::BinaryDataBuffer{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::ImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message");
    STATIC_CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
}

TEST_CASE("isColorImageBinaryData<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ColorImageBinaryData{{}, {}});
    CHECK(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m =
      irsol::protocol::OutMessage(irsol::protocol::Success::from(irsol::protocol::Command{"x"}));
    CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::BinaryDataBuffer{{}, {}});
    CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
  {
    auto m = irsol::protocol::OutMessage(
      irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message"));
    CHECK_FALSE(irsol::protocol::isColorImageBinaryData(m));
  }
}

TEST_CASE("isError<direct>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message");
    STATIC_CHECK(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::Success::from(irsol::protocol::Command{"x"});
    STATIC_CHECK_FALSE(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::BinaryDataBuffer{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::ImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::ColorImageBinaryData{{}, {}};
    STATIC_CHECK_FALSE(irsol::protocol::isError(m));
  }
}

TEST_CASE("isError<variant>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::OutMessage(
      irsol::protocol::Error::from(irsol::protocol::Command{"x"}, "error message"));
    CHECK(irsol::protocol::isError(m));
  }
  {
    auto m =
      irsol::protocol::OutMessage(irsol::protocol::Success::from(irsol::protocol::Command{"x"}));
    CHECK_FALSE(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::BinaryDataBuffer{{}, {}});
    CHECK_FALSE(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::OutMessage(irsol::protocol::ColorImageBinaryData{{}, {}});
    CHECK_FALSE(irsol::protocol::isError(m));
  }
}
