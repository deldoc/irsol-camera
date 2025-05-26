#include "irsol/protocol/serialization/serializer.hpp"

#include "irsol/logging.hpp"
#include "irsol/protocol/utils.hpp"

#include <variant>

namespace irsol {
namespace protocol {

SerializedMessage
Serializer::serialize(OutMessage&& msg)
{

  return std::visit(
    [](auto&& msg) -> SerializedMessage {
      using T = std::decay_t<decltype(msg)>;
      return serialize<T>(std::move(msg));
    },
    msg);
}

std::string
Serializer::serializeValue(internal::value_t&& value)
{
  return std::visit(
    [](auto&& val) -> std::string {
      using T = std::decay_t<decltype(val)>;
      return serializeValue<T>(std::move(val));
    },
    value);
}

SerializedMessage
Serializer::serializeSuccess(Success&& msg)
{
  IRSOL_LOG_TRACE("Serializing Success message: {}", msg.toString());
  std::string result = msg.identifier;
  if(msg.source == InMessageKind::ASSIGNMENT) {
    IRSOL_ASSERT_ERROR(
      msg.hasBody(),
      "Body is not present in 'Success' message, created from 'Assignment'. This should "
      "never happen, as a successful assignment should always provide a body for the "
      "associated Success message.");
    auto body = *msg.body;
    result += "=" + serializeValue(std::move(body)) + "\n";
  } else if(msg.source == InMessageKind::INQUIRY) {
    if(msg.hasBody()) {
      auto body = *msg.body;
      result += "=" + serializeValue(std::move(body)) + "\n";
    } else {
      result += "\n";
    }
  } else if(msg.source == InMessageKind::COMMAND) {
    result += ";\n";
  }
  return {result, {}};
}

SerializedMessage
Serializer::serializeBinaryDataBuffer(BinaryDataBuffer&& msg)
{
  IRSOL_LOG_TRACE("Serializing binary buffer: {}", msg.toString());
  // TODO: implement serialization
  throw std::runtime_error("Binary data serialization not supported");
}

SerializedMessage
Serializer::serializeImageBinaryData(ImageBinaryData&& msg)
{
  IRSOL_LOG_TRACE("Serializing image binary data: {}", msg.toString());
  std::string header = "image_data:3x";
  header += std::to_string(msg.shape[1]) + "x";
  header += std::to_string(msg.shape[0]) + "x";
  header += "1:";
  auto message = SerializedMessage(header, std::move(msg.extractData()));
  return std::move(message);
}

SerializedMessage
Serializer::serializeColorImageBinaryData(ColorImageBinaryData&& msg)
{
  IRSOL_LOG_TRACE("Serializing color image binary data: {}", msg.toString());
  // TODO: implement serialization
  throw std::runtime_error("Binary data serialization not supported");
}

SerializedMessage
Serializer::serializeError(Error&& msg)
{
  IRSOL_LOG_TRACE("Serializing error message: {}", msg.toString());
  return {msg.identifier + ": Error: " + msg.description + "\n", {}};
}

}  // namespace protocol
}  // namespace irsol