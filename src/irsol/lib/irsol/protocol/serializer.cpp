#include "irsol/protocol/serializer.hpp"

#include "irsol/logging.hpp"
#include "irsol/protocol/utils.hpp"

#include <variant>

namespace irsol {
namespace protocol {

const internal::byte_t*
SerializedMessage::headerData() const
{
  return reinterpret_cast<const internal::byte_t*>(header.data());
}

size_t
SerializedMessage::headerSize() const
{
  return header.size();
}

const internal::byte_t*
SerializedMessage::payloadData() const
{
  return payload.data();
}

size_t
SerializedMessage::payloadSize() const
{
  return payload.size();
}

SerializedMessage
Serializer::serialize(const OutMessage& msg)
{

  return std::visit(
    [](auto&& msg) -> SerializedMessage {
      using T = std::decay_t<decltype(msg)>;
      return serialize<T>(msg);
    },
    msg);
}

std::string
Serializer::serializeValue(const internal::value_t& value)
{
  return std::visit(
    [](auto&& val) -> std::string {
      using T = std::decay_t<decltype(val)>;
      return serializeValue<T>(val);
    },
    value);
}

SerializedMessage
Serializer::serializeSuccess(const Success& msg)
{
  IRSOL_LOG_TRACE("Serializing Success message: {}", msg.toString());
  std::string result = msg.identifier;
  if(msg.source == InMessageKind::ASSIGNMENT) {
    IRSOL_ASSERT_ERROR(
      msg.hasBody(),
      "Body is not present in 'Success' message, created from 'Assignment'. This should "
      "never happen, as a successful assignment should always provide a body for the "
      "associated Success message.");
    result += "=" + serializeValue(*msg.body);
  } else if(msg.source == InMessageKind::INQUIRY) {
    if(msg.hasBody()) {
      result += "=" + serializeValue(*msg.body);
    }
  } else if(msg.source == InMessageKind::COMMAND) {
    result += ";";
  }
  return {result};
}

SerializedMessage
Serializer::serializeBinaryDataBuffer(const BinaryDataBuffer& msg)
{
  IRSOL_LOG_TRACE("Serializing binary buffer: {}", msg.toString());
  // TODO: implement serialization
  throw std::runtime_error("Binary data serialization not supported");
}

SerializedMessage
Serializer::serializeImageBinaryData(const ImageBinaryData& msg)
{
  IRSOL_LOG_TRACE("Serializing image binary data: {}", msg.toString());
  // TODO: implement serialization
  throw std::runtime_error("Binary data serialization not supported");
}

SerializedMessage
Serializer::serializeColorImageBinaryData(const ColorImageBinaryData& msg)
{
  IRSOL_LOG_TRACE("Serializing color image binary data: {}", msg.toString());
  // TODO: implement serialization
  throw std::runtime_error("Binary data serialization not supported");
}

SerializedMessage
Serializer::serializeError(const Error& msg)
{
  IRSOL_LOG_TRACE("Serializing error message: {}", msg.toString());
  return {msg.identifier + ": Error: " + msg.description};
}

}  // namespace protocol
}  // namespace irsol