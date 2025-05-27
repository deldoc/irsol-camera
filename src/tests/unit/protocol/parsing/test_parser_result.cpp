#include "irsol/protocol/message.hpp"
#include "irsol/protocol/parsing/parser_result.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("bool(ParserResult)", "[Protocol][Protocol::Parser]")
{
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Command> result(
      irsol::protocol::Command("x"));
    CHECK(bool(result));
  }
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Inquiry> result("error");
    CHECK_FALSE(bool(result));
  }
}

TEST_CASE("ParserResult::isMessage()", "[Protocol][Protocol::Parser]")
{
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Command> result(
      irsol::protocol::Command("x"));
    CHECK(result.isMessage());
  }
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Inquiry> result("error");
    CHECK_FALSE(result.isMessage());
  }
}

TEST_CASE("ParserResult::isError()", "[Protocol][Protocol::Parser]")
{
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Command> result(
      irsol::protocol::Command("x"));
    CHECK_FALSE(result.isError());
  }
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Inquiry> result("error");
    CHECK(result.isError());
  }
}

TEST_CASE("ParserResult<Assignment>::getMessage()", "[Protocol][Protocol::Parser]")
{
  auto value = GENERATE(
    irsol::types::protocol_value_t(1),
    irsol::types::protocol_value_t(3.14),
    irsol::types::protocol_value_t("hello world"));

  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

  // Need to create a copy of the identifier and value for the check,
  // as these are moved into the ParserResult.
  auto expectedIdentifier = identifier;
  auto expected_value     = value;

  irsol::protocol::Assignment                                          m(identifier, value);
  irsol::protocol::internal::ParserResult<irsol::protocol::Assignment> result(std::move(m));
  CHECK(result.isMessage());
  CHECK(result.getMessage().identifier == expectedIdentifier);
  CHECK(result.getMessage().value == expected_value);
}

TEST_CASE("ParserResult<Inquiry>::getMessage()", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

  // Need to create a copy of the identifier for the check,
  // as these are moved into the ParserResult.
  auto expectedIdentifier = identifier;

  irsol::protocol::Inquiry                                          m(identifier);
  irsol::protocol::internal::ParserResult<irsol::protocol::Inquiry> result(std::move(m));
  CHECK(result.isMessage());
  CHECK(result.getMessage().identifier == expectedIdentifier);
}

TEST_CASE("ParserResult<Command>::getMessage()", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

  // Need to create a copy of the identifier for the check,
  // as these are moved into the ParserResult.
  auto expectedIdentifier = identifier;

  irsol::protocol::Command                                          m(identifier);
  irsol::protocol::internal::ParserResult<irsol::protocol::Command> result(std::move(m));
  CHECK(result.isMessage());
  CHECK(result.getMessage().identifier == expectedIdentifier);
}

TEST_CASE("ParserResult<T>::getError()", "[Protocol][Protocol::Parser]")
{
  auto errorMessage = GENERATE("invalid syntax", "unknown identifier", "unexpected character");
  auto expectedErrorMessage = errorMessage;
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Command> result(errorMessage);
    CHECK(result.isError());
    CHECK(result.getError() == expectedErrorMessage);
  }
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Inquiry> result(errorMessage);
    CHECK(result.isError());
    CHECK(result.getError() == expectedErrorMessage);
  }
  {
    irsol::protocol::internal::ParserResult<irsol::protocol::Assignment> result(errorMessage);
    CHECK(result.isError());
    CHECK(result.getError() == expectedErrorMessage);
  }
}