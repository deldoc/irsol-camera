#include "irsol/protocol/message/inquiry.hpp"

#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

TEST_CASE("Inquiry::Inquiry()", "[Protocol][Protocol::Message]")
{

  {
    auto identifier = GENERATE(
      "x", "it", "long_identifier", "sequence_identifier[4]", "nested_sequence_identifier[4][423]");

    irsol::protocol::Inquiry m(identifier);
    CHECK(m.identifier == identifier);
  }
  {
    auto identifier =
      GENERATE("", " ", " x", "2", "4x", "identifier with space", "identifier-with-dash");
    CHECK_THROWS_AS(irsol::protocol::Inquiry(identifier), std::invalid_argument);
  }
}
