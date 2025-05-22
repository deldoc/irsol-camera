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