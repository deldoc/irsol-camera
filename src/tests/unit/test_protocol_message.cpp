#include "irsol/protocol/message.hpp"

#include <catch2/catch_all.hpp>
#include <string>

TEST_CASE("Assignment<int>.hasInt()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto value      = GENERATE(42, 5, 32121);

  irsol::protocol::Assignment m{identifier, value};
  CHECK(m.hasInt());
  CHECK_FALSE(m.hasDouble());
  CHECK_FALSE(m.hasString());
}

TEST_CASE("Assignment<double>.hasDouble()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto value      = GENERATE(42.0123, 5.0, 32121.9999);

  irsol::protocol::Assignment m{identifier, value};
  CHECK_FALSE(m.hasInt());
  CHECK(m.hasDouble());
  CHECK_FALSE(m.hasString());
}

TEST_CASE("Assignment<string>.hasString()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto value      = GENERATE("c", "longer", "long_string_with_underscores", "5è4?-é");

  irsol::protocol::Assignment m{identifier, std::string(value)};
  CHECK_FALSE(m.hasInt());
  CHECK_FALSE(m.hasDouble());
  CHECK(m.hasString());
}

TEST_CASE("Status.hasBody()", "[Protocol][Protocol::Message]")
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

TEST_CASE("getMessageKind<InMessage>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    CHECK(irsol::protocol::getMessageKind(m) == irsol::protocol::InMessageKind::ASSIGNMENT);
  }
  {
    auto m = irsol::protocol::Inquiry{"x"};
    CHECK(irsol::protocol::getMessageKind(m) == irsol::protocol::InMessageKind::INQUIRY);
  }
  {
    auto m = irsol::protocol::Command{"x"};
    CHECK(irsol::protocol::getMessageKind(m) == irsol::protocol::InMessageKind::COMMAND);
  }
}

TEST_CASE("getMessageKind<OutMessage>()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Status{"x", std::make_optional("body")};
    CHECK(irsol::protocol::getMessageKind(m) == irsol::protocol::OutMessageKind::STATUS);
  }
  {
    auto m = irsol::protocol::Status{"x"};
    CHECK(irsol::protocol::getMessageKind(m) == irsol::protocol::OutMessageKind::STATUS);
  }
  {
    auto m = irsol::protocol::Error{"x", "error message"};
    CHECK(irsol::protocol::getMessageKind(m) == irsol::protocol::OutMessageKind::ERROR);
  }
}

TEST_CASE("isAssignment()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    CHECK(irsol::protocol::isAssignment(m));
  }
  {
    auto m = irsol::protocol::Inquiry{"x"};
    CHECK_FALSE(irsol::protocol::isAssignment(m));
  }
  {
    auto m = irsol::protocol::Command{"x"};
    CHECK_FALSE(irsol::protocol::isAssignment(m));
  }
}

TEST_CASE("isInquiry()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Inquiry{"x"};
    CHECK(irsol::protocol::isInquiry(m));
  }
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    CHECK_FALSE(irsol::protocol::isInquiry(m));
  }
  {
    auto m = irsol::protocol::Command{"x"};
    CHECK_FALSE(irsol::protocol::isInquiry(m));
  }
}

TEST_CASE("isCommand()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Command{"x"};
    CHECK(irsol::protocol::isCommand(m));
  }
  {
    auto m = irsol::protocol::Inquiry{"x"};
    CHECK_FALSE(irsol::protocol::isCommand(m));
  }
  {
    auto m = irsol::protocol::Assignment{"x", 42};
    CHECK_FALSE(irsol::protocol::isCommand(m));
  }
}

TEST_CASE("isStatus()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Status{"x", std::make_optional("body")};
    CHECK(irsol::protocol::isStatus(m));
  }
  {
    auto m = irsol::protocol::Status{"x"};
    CHECK(irsol::protocol::isStatus(m));
  }
  {
    auto m = irsol::protocol::Error{"x", "error message"};
    CHECK_FALSE(irsol::protocol::isStatus(m));
  }
}

TEST_CASE("isError()", "[Protocol][Protocol::Message]")
{
  {
    auto m = irsol::protocol::Error{"x", "error message"};
    CHECK(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::Status{"x", std::make_optional("body")};
    CHECK_FALSE(irsol::protocol::isError(m));
  }
  {
    auto m = irsol::protocol::Status{"x"};
    CHECK_FALSE(irsol::protocol::isError(m));
  }
}