#include "irsol/protocol/message/error.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

TEST_CASE("Error::from<T>()", "[Protocol][Protocol::Message]")
{
  auto identifier  = GENERATE("x", "it", "long_identifier");
  auto description = GENERATE("hello world", "error description");
  {
    auto value = GENERATE(
      irsol::protocol::internal::value_t{42},
      irsol::protocol::internal::value_t{3.15},
      irsol::protocol::internal::value_t{"my string"});
    irsol::protocol::Assignment assignment{identifier, value};
    auto                        result = irsol::protocol::Error::from(assignment, description);
    CHECK(result.identifier == identifier);
    CHECK(result.source == irsol::protocol::InMessageKind::ASSIGNMENT);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     result = irsol::protocol::Error::from(inquiry, description);
    CHECK(result.identifier == identifier);
    CHECK(result.source == irsol::protocol::InMessageKind::INQUIRY);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::Command command{identifier};
    auto                     result = irsol::protocol::Error::from(command, description);
    CHECK(result.identifier == identifier);
    CHECK(result.source == irsol::protocol::InMessageKind::COMMAND);
    CHECK(result.description == description);
  }
}

TEST_CASE("Error::from<InMessage>()", "[Protocol][Protocol::Message]")
{
  auto identifier  = GENERATE("x", "it", "long_identifier");
  auto description = GENERATE("hello world", "error description");

  {
    auto value = GENERATE(
      irsol::protocol::internal::value_t{42},
      irsol::protocol::internal::value_t{3.15},
      irsol::protocol::internal::value_t{"my string"});
    irsol::protocol::Assignment assignment{identifier, value};
    irsol::protocol::InMessage  assignmentInMessage(assignment);
    auto result = irsol::protocol::Error::from(assignmentInMessage, description);
    CHECK(result.identifier == identifier);
    CHECK(result.source == irsol::protocol::InMessageKind::ASSIGNMENT);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::Inquiry   inquiry{identifier};
    irsol::protocol::InMessage inquiryInMessage(inquiry);
    auto                       result = irsol::protocol::Error::from(inquiryInMessage, description);
    CHECK(result.identifier == identifier);
    CHECK(result.source == irsol::protocol::InMessageKind::INQUIRY);
    CHECK(result.description == description);
  }
  {
    irsol::protocol::Command   command{identifier};
    irsol::protocol::InMessage commandInMessage(command);
    auto                       result = irsol::protocol::Error::from(commandInMessage, description);
    CHECK(result.identifier == identifier);
    CHECK(result.source == irsol::protocol::InMessageKind::COMMAND);
    CHECK(result.description == description);
  }
}
