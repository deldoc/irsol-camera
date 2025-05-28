#include "irsol/protocol/message.hpp"
#include "irsol/protocol/serialization/serializer.hpp"

#include <catch2/catch_all.hpp>
#include <utility>

TEST_CASE("Serializer::message_termination", "[Protocol][Protocol::Serialization]")
{
  CHECK(std::string(irsol::protocol::Serializer::message_termination) == "\n");
}
TEST_CASE("Serializer::serializeValue<direct>()", "[Protocol][Protocol::Serialization]")
{
  {
    // From an integer
    int         value      = GENERATE(1, 2, 3, 4, 5);
    std::string expected   = std::to_string(value);
    std::string serialized = irsol::protocol::Serializer::serializeValue(std::move(value));
    CHECK(serialized == expected);
  }
  {
    // From a double
    double      value      = GENERATE(1.0, 2.5, 3.14, 4.2, 5.0, -31.4, 0.0);
    std::string expected   = std::to_string(value);
    std::string serialized = irsol::protocol::Serializer::serializeValue(std::move(value));
    CHECK(serialized == expected);
  }
  {
    // From a string
    std::string value      = GENERATE("", "hello", "world", "test", "12345");
    std::string expected   = "{" + value + "}";
    std::string serialized = irsol::protocol::Serializer::serializeValue(std::move(value));
    CHECK(serialized == expected);
  }
}

TEST_CASE("Serializer::serializeValue<variant>()", "[Protocol][Protocol::Serialization]")
{
  {
    // From an integer
    int                            value = GENERATE(1, 2, 3, 4, 5);
    irsol::types::protocol_value_t variantValue(value);
    std::string                    expected = std::to_string(value);
    std::string serialized = irsol::protocol::Serializer::serializeValue(std::move(variantValue));
    CHECK(serialized == expected);
  }
  {
    // From a double
    double                         value = GENERATE(1.0, 2.5, 3.14, 4.2, 5.0, -31.4, 0.0);
    irsol::types::protocol_value_t variantValue(value);
    std::string                    expected = std::to_string(value);
    std::string serialized = irsol::protocol::Serializer::serializeValue(std::move(variantValue));
    CHECK(serialized == expected);
  }
  {
    // From a string
    auto [value, expected] = GENERATE(
      std::make_pair("", "{}"),
      std::make_pair("hello", "{hello}"),
      std::make_pair("world", "{world}"),
      std::make_pair("test", "{test}"),
      std::make_pair("12345", "{12345}"));
    irsol::types::protocol_value_t variantValue(value);
    std::string serialized = irsol::protocol::Serializer::serializeValue(std::move(variantValue));
    CHECK(serialized == expected);
  }
}

TEST_CASE("Serializer::serialize<direct>(Success)", "[Protocol][Protocol::Serialization]")
{
  {
    // From a successful Assignment
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    auto value = GENERATE(
      irsol::types::protocol_value_t{42},
      irsol::types::protocol_value_t{3.15},
      irsol::types::protocol_value_t{"my string"});

    irsol::protocol::Assignment assignment{identifier, value};
    irsol::protocol::Success    success = irsol::protocol::Success::from(assignment);

    std::string expectedHeader =
      std::string(identifier) + "=" +
      irsol::protocol::Serializer::serializeValue(irsol::types::protocol_value_t(value)) +
      irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(success));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From a successful Inquiry
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    auto inquery_result = GENERATE(
      irsol::types::protocol_value_t{42},
      irsol::types::protocol_value_t{3.15},
      irsol::types::protocol_value_t{"my string"});
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     success = irsol::protocol::Success::from(inquiry, inquery_result);

    std::string expectedHeader =
      std::string(identifier) + "=" +
      irsol::protocol::Serializer::serializeValue(irsol::types::protocol_value_t(inquery_result)) +
      irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(success));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From a successful Command
    auto                     identifier = GENERATE("x", "it", "long_identifier");
    irsol::protocol::Command command{identifier};
    auto                     success = irsol::protocol::Success::from(command);

    std::string expectedHeader = std::string(identifier) + ";\n";
    auto        serialized     = irsol::protocol::Serializer::serialize(std::move(success));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
}

TEST_CASE("Serializer::serialize<variant>(Success)", "[Protocol][Protocol::Serialization]")
{
  {
    // From a successful Assignment
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    auto value = GENERATE(
      irsol::types::protocol_value_t{42},
      irsol::types::protocol_value_t{3.15},
      irsol::types::protocol_value_t{"my string"});
    irsol::protocol::Assignment assignment{identifier, value};
    auto                        success        = irsol::protocol::Success::from(assignment);
    auto                        successVariant = irsol::protocol::OutMessage(success);

    std::string expectedHeader =
      std::string(identifier) + "=" +
      irsol::protocol::Serializer::serializeValue(irsol::types::protocol_value_t(value)) +
      irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(successVariant));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From a successful Inquiry
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    auto inquery_result = GENERATE(
      irsol::types::protocol_value_t{42},
      irsol::types::protocol_value_t{3.15},
      irsol::types::protocol_value_t{"my string"});
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     success = irsol::protocol::Success::from(inquiry, inquery_result);
    auto                     successVariant = irsol::protocol::OutMessage(success);

    std::string expectedHeader =
      std::string(identifier) + "=" +
      irsol::protocol::Serializer::serializeValue(irsol::types::protocol_value_t(inquery_result)) +
      irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(successVariant));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From a successful Command
    auto                     identifier = GENERATE("x", "it", "long_identifier");
    irsol::protocol::Command command{identifier};
    auto                     success        = irsol::protocol::Success::from(command);
    auto                     successVariant = irsol::protocol::OutMessage(success);

    std::string expectedHeader = std::string(identifier) + ";\n";
    auto        serialized     = irsol::protocol::Serializer::serialize(std::move(successVariant));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
}

TEST_CASE("Serializer::serialize<direct>(Error)", "[Protocol][Protocol::Serialization]")
{
  auto description = GENERATE("", "hello world", "error description");
  {
    // From an erroneous Assignment
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    auto value = GENERATE(
      irsol::types::protocol_value_t{42},
      irsol::types::protocol_value_t{3.15},
      irsol::types::protocol_value_t{"my string"});

    irsol::protocol::Assignment assignment{identifier, value};
    auto                        error = irsol::protocol::Error::from(assignment, description);

    std::string expectedHeader = std::string(identifier) + ": Error: " + description +
                                 irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(error));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From an erroneous Inquiry
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     error = irsol::protocol::Error::from(inquiry, description);

    std::string expectedHeader = std::string(identifier) + ": Error: " + description +
                                 irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(error));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From an erroneous Command
    auto                     identifier = GENERATE("x", "it", "long_identifier");
    irsol::protocol::Command command{identifier};
    auto                     error = irsol::protocol::Error::from(command, description);

    std::string expectedHeader = std::string(identifier) + ": Error: " + description +
                                 irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(error));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
}
TEST_CASE("Serializer::serialize<variant>(Error)", "[Protocol][Protocol::Serialization]")
{
  auto description = GENERATE("", "hello world", "error description");
  {
    // From an erroneous Assignment
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    auto value = GENERATE(
      irsol::types::protocol_value_t{42},
      irsol::types::protocol_value_t{3.15},
      irsol::types::protocol_value_t{"my string"});

    irsol::protocol::Assignment assignment{identifier, value};
    auto                        error = irsol::protocol::Error::from(assignment, description);
    auto                        errorVariant = irsol::protocol::OutMessage(error);

    std::string expectedHeader = std::string(identifier) + ": Error: " + description +
                                 irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(errorVariant));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From an erroneous Inquiry
    auto identifier =
      GENERATE("x", "it", "long_identifier", "sequence_identifier[5]", "nested_identifier[3][2]");
    irsol::protocol::Inquiry inquiry{identifier};
    auto                     error        = irsol::protocol::Error::from(inquiry, description);
    auto                     errorVariant = irsol::protocol::OutMessage(error);

    std::string expectedHeader = std::string(identifier) + ": Error: " + description +
                                 irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(errorVariant));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
  {
    // From an erroneous Command
    auto                     identifier = GENERATE("x", "it", "long_identifier");
    irsol::protocol::Command command{identifier};
    auto                     error        = irsol::protocol::Error::from(command, description);
    auto                     errorVariant = irsol::protocol::OutMessage(error);

    std::string expectedHeader = std::string(identifier) + ": Error: " + description +
                                 irsol::protocol::Serializer::message_termination;
    auto serialized = irsol::protocol::Serializer::serialize(std::move(errorVariant));
    CHECK(serialized.header == expectedHeader);
    CHECK_FALSE(serialized.hasPayload());
    CHECK(serialized.payloadSize() == 0);
  }
}
