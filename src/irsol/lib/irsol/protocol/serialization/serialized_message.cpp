#include "irsol/protocol/serialization/serialized_message.hpp"

#include <sstream>

namespace irsol {
namespace protocol {

SerializedMessage::SerializedMessage(
  const std::string&              header,
  std::vector<internal::byte_t>&& payload)
  : header(header), payload(std::move(payload))
{}

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

std::string
SerializedMessage::toString() const
{
  std::ostringstream oss;
  oss << "SerializedMessage{"
      << "header: '" << header << "', payload size: " << payload.size() << "bytes}";
  return oss.str();
}

}  // namespace protocol
}  // namespace irsol