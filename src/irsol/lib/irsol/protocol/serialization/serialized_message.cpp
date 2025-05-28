#include "irsol/protocol/serialization/serialized_message.hpp"

#include <sstream>

namespace irsol {
namespace protocol {
namespace internal {

SerializedMessage::SerializedMessage(
  const std::string&                  header,
  std::vector<irsol::types::byte_t>&& payload)
  : header(header), payload(std::move(payload))
{}

bool
SerializedMessage::hasHeader() const
{
  return header.size() > 0;
}

bool
SerializedMessage::hasPayload() const
{
  return payloadSize() > 0;
}

size_t
SerializedMessage::payloadSize() const
{
  return payload.size();
}
std::string
SerializedMessage::toString() const
{
  std::ostringstream oss;
  oss << "SerializedMessage{"
      << "header: '" << header << "'";
  if(hasPayload()) {
    oss << ", payload size: " << payload.size() << "bytes";
  } else {
    oss << ", no payload";
  }
  oss << "}";
  return oss.str();
}
}  // namespace internal
}  // namespace protocol
}  // namespace irsol