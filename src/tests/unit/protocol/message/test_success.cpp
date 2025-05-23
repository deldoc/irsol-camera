#include "irsol/protocol/message/success.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

TEST_CASE("Success::Success()", "[Protocol][Protocol::Message]")
{
  auto body = GENERATE(std::optional<irsol::protocol::internal::value_t>("hello world"));
  {
    auto identifier = GENERATE(
      "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

    irsol::protocol::Success m1(identifier, body);
    CHECK(m1.identifier == identifier);
    CHECK(m1.body == body);
    irsol::protocol::Success m2(identifier);
    CHECK(m2.identifier == identifier);
    CHECK(m2.body == std::nullopt);
  }
  {
    auto identifier =
      GENERATE("", " ", " x", "2", "4x", "identifier with space", "identifier-with-dash");
    CHECK_THROWS_AS(irsol::protocol::Success(identifier, body), std::invalid_argument);
    CHECK_THROWS_AS(irsol::protocol::Success(identifier), std::invalid_argument);
  }
}

TEST_CASE("Success::hasBody()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");

  {
    irsol::protocol::Success m{identifier, std::make_optional("body")};
    CHECK(m.hasBody());
  }
  {
    irsol::protocol::Success m{identifier, std::nullopt};
    CHECK_FALSE(m.hasBody());
  }
  {
    irsol::protocol::Success m{identifier};
    CHECK_FALSE(m.hasBody());
  }
}

TEST_CASE("Success::from<direct>()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  {
    auto value = GENERATE(
      irsol::protocol::internal::value_t{42},
      irsol::protocol::internal::value_t{3.15},
      irsol::protocol::internal::value_t{"my string"});
    irsol::protocol::Assignment assignment{identifier, value};
    auto                        result = irsol::protocol::Success::from(assignment);
    CHECK(result.identifier == identifier);
    CHECK(result.hasBody());
    CHECK(*result.body == value);
  }
  {
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     result = irsol::protocol::Success::from(inquiry);
    CHECK(result.identifier == identifier);
  }
  {
    irsol::protocol::Command command{identifier};
    auto                     result = irsol::protocol::Success::from(command);
    CHECK(result.identifier == identifier);
  }
}
TEST_CASE("Success::from<variant>()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  {
    auto value = GENERATE(
      irsol::protocol::internal::value_t{42},
      irsol::protocol::internal::value_t{3.15},
      irsol::protocol::internal::value_t{"my string"});
    irsol::protocol::InMessage message{irsol::protocol::Assignment{identifier, value}};
    auto                       result = irsol::protocol::Success::from(message);
    CHECK(result.identifier == identifier);
    CHECK(result.hasBody());
    CHECK(*result.body == value);
  }
  {
    irsol::protocol::InMessage message{irsol::protocol::Inquiry{identifier}};
    auto                       result = irsol::protocol::Success::from(message);
    CHECK(result.identifier == identifier);
  }
  {
    irsol::protocol::InMessage message{irsol::protocol::Command{identifier}};
    auto                       result = irsol::protocol::Success::from(message);
    CHECK(result.identifier == identifier);
  }
}