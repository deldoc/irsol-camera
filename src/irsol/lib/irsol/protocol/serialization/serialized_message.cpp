#include "irsol/protocol/serialization/serialized_message.hpp"

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

}  // namespace protocol
}  // namespace irsol