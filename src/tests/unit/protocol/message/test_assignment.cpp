#include "irsol/protocol/message/assignment.hpp"

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
