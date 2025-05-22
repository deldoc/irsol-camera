#include "irsol/protocol/utils.hpp"

#include <catch2/catch_all.hpp>
#include <string>

TEST_CASE("fromString<int>", "[Protocol][Protocol::Utils]")
{
  auto value = GENERATE(0, 1, 42, 31232423);

  auto resValue = irsol::protocol::utils::fromString<int>(std::to_string(value));
  REQUIRE(value == resValue);
}

TEST_CASE("fromString<int> with invalid arg", "[Protocol][Protocol::Utils]")
{
  auto value =
    GENERATE("", " ", "-", "42.", "42.0", "42i", "42.i", ".42", "i42", "42..", "invalid");

  REQUIRE_THROWS_AS(irsol::protocol::utils::fromString<int>(value), std::invalid_argument);
}

TEST_CASE("fromString<double>", "[Protocol][Protocol::Utils]")
{
  auto value = GENERATE(0.0, 1.0, 1.0001, 42.43242, 31232423.123);

  auto resValue = irsol::protocol::utils::fromString<double>(std::to_string(value));
  REQUIRE(value == Catch::Approx(resValue));
}

TEST_CASE("fromString<double>->throw", "[Protocol][Protocol::Utils]")
{
  auto value = GENERATE("", " ", "-", "42i", "42.i", "i42", "42..", "invalid");

  REQUIRE_THROWS_AS(irsol::protocol::utils::fromString<double>(value), std::invalid_argument);
}

TEST_CASE("fromString<string>", "[Protocol][Protocol::Utils]")
{
  auto value = GENERATE("", "h", "hello", "hello world", "4.s", "s3");

  auto resValue = irsol::protocol::utils::fromString<std::string>(value);
  REQUIRE(value == resValue);
}

TEST_CASE("trim", "[Protocol][Protocol::Utils]")
{
  auto value = GENERATE(
    "hello world",
    " hello world",
    "hello world ",
    " hello world\n",
    " hello world\t",
    "\n\t  \t\t  \n\r hello world\n \r   \r");

  auto trimmedValue = irsol::protocol::utils::trim(value);
  REQUIRE(trimmedValue == "hello world");
}
