#include "irsol/protocol/message.hpp"
#include "irsol/protocol/parser.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("Parsing invalid identifier", "[Protocol][Protocol::Parser]")
{
  // Invalid identifiers
  auto identifier = GENERATE(
    ""
    "identifier with spaces",
    "identifier-with-dashes",
    "non-alphanumeric<",
    "??");

  // Valid suffixes (for assignments, queries and commands)
  auto suffix = GENERATE("=value", "=42", "=3.14", "?", "");

  std::string line = std::string(identifier) + suffix;

  auto msg = irsol::protocol::Parser::parse(line);
  REQUIRE(!msg.has_value());
}

TEST_CASE("Assignment parsing int", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");
  auto value = GENERATE(42, 5, 32121);

  std::string input = std::string(identifier) + "=" + std::to_string(value);

  auto msg = irsol::protocol::Parser::parse(input);
  REQUIRE(msg.has_value());

  auto asg = std::get<irsol::protocol::Assignment>(*msg);

  CHECK(asg.identifier == identifier);
  CHECK(std::get<int>(asg.value) == value);
}

TEST_CASE("Assignment parsing double", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");
  auto value = GENERATE(42.0, 0, 5, 32121.4234234);

  std::string input = std::string(identifier) + "=" + std::to_string(value);

  auto msg = irsol::protocol::Parser::parse(input);
  REQUIRE(msg.has_value());

  auto asg = std::get<irsol::protocol::Assignment>(*msg);

  CHECK(asg.identifier == identifier);
  CHECK(std::get<double>(asg.value) == Catch::Approx(value));
}

TEST_CASE("Assignment parsing string", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");
  auto value = GENERATE("c", "longer", "long_string_with_underscores", "5è4?-é");

  std::string input = std::string(identifier) + "=" + value;

  auto msg = irsol::protocol::Parser::parse(input);
  REQUIRE(msg.has_value());

  auto asg = std::get<irsol::protocol::Assignment>(*msg);

  CHECK(asg.identifier == identifier);
  CHECK(std::get<std::string>(asg.value) == value);
}
TEST_CASE("Assignment parsing string (quoted)", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");
  auto value             = GENERATE("c", "longer", "long_string_with_underscores", "5è4?-é");
  auto prefixSuffixChars = GENERATE("\"\"", "''", "{}", "  ");

  std::string input =
    std::string(identifier) + "=" + prefixSuffixChars[0] + value + prefixSuffixChars[1];

  auto msg = irsol::protocol::Parser::parse(input);
  REQUIRE(msg.has_value());

  auto asg = std::get<irsol::protocol::Assignment>(*msg);

  CHECK(asg.identifier == identifier);
  CHECK(std::get<std::string>(asg.value) == value);
}

TEST_CASE("Inquiry parsing", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE(
    "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

  std::string input = std::string(identifier) + "?";
  auto        msg   = irsol::protocol::Parser::parse(input);
  REQUIRE(msg.has_value());
  auto inq = std::get<irsol::protocol::Inquiry>(*msg);
  CHECK(inq.identifier == identifier);
}

TEST_CASE("Command parsing", "[Protocol][Protocol::Parser]")
{
  auto identifier = GENERATE("x", "it", "long_identifier");

  std::string input = std::string(identifier);
  auto        msg   = irsol::protocol::Parser::parse(input);
  REQUIRE(msg.has_value());
  auto inq = std::get<irsol::protocol::Command>(*msg);
  CHECK(inq.identifier == identifier);
}
