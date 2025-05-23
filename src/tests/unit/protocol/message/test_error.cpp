#include "irsol/protocol/message/error.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>


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

TEST_CASE("Error::from<direct>()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto description = GENERATE("hello world", "error description");
  {
    auto value = GENERATE(
      irsol::protocol::internal::value_t{42},
      irsol::protocol::internal::value_t{3.15},
      irsol::protocol::internal::value_t{"my string"});
    irsol::protocol::Assignment assignment{identifier, value};
    auto                        result = irsol::protocol::Error::from(assignment, description);
    CHECK(result.identifier == identifier);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     result = irsol::protocol::Error::from(inquiry, description);
    CHECK(result.identifier == identifier);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::Command command{identifier};
    auto                     result = irsol::protocol::Error::from(command, description);
    CHECK(result.identifier == identifier);
    CHECK(result.description == description);
  }
}
TEST_CASE("Error::from<variant>()", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto description = GENERATE("hello world", "error description");
  {
    auto value = GENERATE(
      irsol::protocol::internal::value_t{42},
      irsol::protocol::internal::value_t{3.15},
      irsol::protocol::internal::value_t{"my string"});
    irsol::protocol::InMessage message{irsol::protocol::Assignment{identifier, value}};
    auto                       result = irsol::protocol::Error::from(message, description);
    CHECK(result.identifier == identifier);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::InMessage message{irsol::protocol::Inquiry{identifier}};
    auto                       result = irsol::protocol::Error::from(message, description);
    CHECK(result.identifier == identifier);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::InMessage message{irsol::protocol::Command{identifier}};
    auto                       result = irsol::protocol::Error::from(message, description);
    CHECK(result.identifier == identifier);
    CHECK(result.description == description);
  }
}
