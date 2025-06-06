#include "irsol/protocol/serialization/serializer.hpp"

#include "irsol/camera/pixel_format.hpp"
#include "irsol/logging.hpp"
#include "irsol/protocol/utils.hpp"
#include "irsol/utils.hpp"

#include <variant>

namespace irsol {
namespace protocol {

internal::SerializedMessage
Serializer::serialize(OutMessage&& msg)
{

  return std::visit(
    [](auto&& msg) -> internal::SerializedMessage {
      using T = std::decay_t<decltype(msg)>;
      return serialize<T>(std::move(msg));
    },
    msg);
}

std::string
Serializer::serializeValue(irsol::types::protocol_value_t&& value)
{
  return std::visit(
    [](auto&& val) -> std::string {
      using T = std::decay_t<decltype(val)>;
      return serializeValue<T>(std::move(val));
    },
    value);
}

internal::SerializedMessage
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
    result += "=" + serializeValue(std::move(body)) + Serializer::message_termination;
  } else if(msg.source == InMessageKind::INQUIRY) {
    if(msg.hasBody()) {
      auto body = *msg.body;
      result += "=" + serializeValue(std::move(body)) + Serializer::message_termination;
    } else {
      result += Serializer::message_termination;
    }
  } else if(msg.source == InMessageKind::COMMAND) {
    result += ";\n";
  }
  return {result, {}};
}

internal::SerializedMessage
Serializer::serializeBinaryDataBuffer(BinaryDataBuffer&& msg)
{
  IRSOL_LOG_TRACE("Serializing binary buffer: {}", msg.toString());
  // TODO: implement serialization
  throw std::runtime_error("Binary data serialization not supported");
}

internal::SerializedMessage
Serializer::serializeImageBinaryData(ImageBinaryData&& msg)
{
  IRSOL_LOG_TRACE("Serializing image binary data: {}", msg.toString());

  std::vector<irsol::types::byte_t> payload;
  payload.reserve(msg.data.size() + 128);  // Reserve extra for header/meta

  {
    std::string imagePrefix        = "img=";
    auto        imagePrefixAsBytes = irsol::utils::stringToBytes(imagePrefix);
    std::move(imagePrefixAsBytes.begin(), imagePrefixAsBytes.end(), std::back_inserter(payload));
  }
  payload.emplace_back(Serializer::SpecialBytes::SOH);
  {
    std::stringstream ss;
    ss << "u" << msg.BYTES_PER_ELEMENT * 8 << "[" << msg.shape[0] << "," << msg.shape[1] << "]";
    for(auto& att : msg.attributes) {
      ss << " " << serializeBinaryDataAttribute(std::move(att));
    }
    auto attributesString = ss.str();
    IRSOL_LOG_DEBUG(
      "Attributes for message '{}' serialized to {}", msg.toString(), attributesString);
    auto attributesStringAsBytes = irsol::utils::stringToBytes(attributesString);

    payload.insert(payload.end(), attributesStringAsBytes.begin(), attributesStringAsBytes.end());
  }
  payload.emplace_back(Serializer::SpecialBytes::STX);

  // Copy image data to the end of the payload buffer
  size_t dataOffset = payload.size();
  payload.resize(payload.size() + msg.data.size());
  std::memcpy(&payload[dataOffset], msg.data.data(), msg.data.size());

  // Swap bytes in-place for 16-bit data (assume always 16-bit)
  // This swaps each pair of bytes in the image data region of the payload.
  irsol::camera::PixelByteSwapper<true>()(
    payload.begin() + static_cast<std::ptrdiff_t>(dataOffset), payload.end());

  payload.emplace_back(Serializer::SpecialBytes::ETX);

  auto message = internal::SerializedMessage("", std::move(payload));
  return std::move(message);
}

internal::SerializedMessage
Serializer::serializeColorImageBinaryData(ColorImageBinaryData&& msg)
{
  IRSOL_LOG_TRACE("Serializing color image binary data: {}", msg.toString());
  // TODO: implement serialization
  throw std::runtime_error("Binary data serialization not supported");
}

internal::SerializedMessage
Serializer::serializeError(Error&& msg)
{
  IRSOL_LOG_TRACE("Serializing error message: {}", msg.toString());
  return {msg.identifier + ": Error: " + msg.description + Serializer::message_termination, {}};
}

std::string
Serializer::serializeBinaryDataAttribute(irsol::protocol::BinaryDataAttribute&& att)
{
  std::stringstream ss;
  ss << att.identifier << "=" << serializeValue(std::move(att.value));
  return ss.str();
}

}  // namespace protocol
}  // namespace irsol