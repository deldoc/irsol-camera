#include "irsol/protocol/message.hpp"
#include "irsol/protocol/serializer.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("Serializer::serialize(Success)", "[Protocol][Protocol::Serializer]")
{
  //   {
  //     irsol::protocol::Success status("myIdentifier", "myBody");
  //     auto                    serialized = irsol::protocol::Serializer::serialize(status);
  //     CHECK(serialized == "myIdentifier:myBody");
  //   }
}
