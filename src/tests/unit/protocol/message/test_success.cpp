#include "irsol/protocol/message/success.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

TEST_CASE("Success::from(Assignment)", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  auto value      = GENERATE(
    irsol::types::protocol_value_t{42},
    irsol::types::protocol_value_t{3.15},
    irsol::types::protocol_value_t{"my string"});
  irsol::protocol::Assignment assignment{identifier, value};
  {
    // without overriding
    auto result = irsol::protocol::Success::from(assignment);
    CHECK(result.identifier == identifier);
    CHECK(result.hasBody());
    CHECK(result.source == irsol::protocol::InMessageKind::ASSIGNMENT);
    CHECK(*result.body == value);
  }
  {
    // with overriding
    auto valueOverride = GENERATE(
      irsol::types::protocol_value_t{41},
      irsol::types::protocol_value_t{3.25},
      irsol::types::protocol_value_t{"my long string"});
    auto result = irsol::protocol::Success::from(assignment, valueOverride);
    CHECK(result.identifier == identifier);
    CHECK(result.hasBody());
    CHECK(result.source == irsol::protocol::InMessageKind::ASSIGNMENT);
    CHECK(*result.body == valueOverride);
  }
}
TEST_CASE("Success::from(Inquiry)", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  {
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     inquiry_result = GENERATE(
      irsol::types::protocol_value_t{42},
      irsol::types::protocol_value_t{3.15},
      irsol::types::protocol_value_t{"my string"});
    auto result = irsol::protocol::Success::from(inquiry, inquiry_result);
    CHECK(result.identifier == identifier);
    CHECK(result.hasBody());
    CHECK(result.source == irsol::protocol::InMessageKind::INQUIRY);
    CHECK(*result.body == inquiry_result);
  }
}
TEST_CASE("Success::from(Command)", "[Protocol][Protocol::Message]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");
  {
    irsol::protocol::Command command{identifier};
    auto                     result = irsol::protocol::Success::from(command);
    CHECK(result.identifier == identifier);
    CHECK_FALSE(result.hasBody());
  }
}