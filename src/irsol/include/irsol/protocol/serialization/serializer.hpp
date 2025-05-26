#pragma once

#include "irsol/assert.hpp"
#include "irsol/protocol/message.hpp"

#include <string>
#include <vector>

namespace irsol {
namespace protocol {

struct SerializedMessage
{
  std::string                   header;
  std::vector<internal::byte_t> payload{};

  SerializedMessage(SerializedMessage&&) noexcept = default;
  SerializedMessage& operator=(SerializedMessage&&) noexcept = default;

  // Delete copy constructor and copy assignment
  SerializedMessage(const SerializedMessage&) = delete;
  SerializedMessage& operator=(const SerializedMessage&) = delete;

  const internal::byte_t* headerData() const;

  size_t headerSize() const;

  const internal::byte_t* payloadData() const;

  size_t payloadSize() const;
};

class Serializer
{
public:
  /*
   * Serialize an OutMessage into a string.
   *
   * @param msg The message to serialize.
   * @return A SerializedMessage containing the serialized message.
   */
  static SerializedMessage serialize(const OutMessage& msg);

  /**
   * Serialize a specific OutMessage type into a string.
   *
   * @param msg The message to serialize. Must be a class used in the OutMessage variant.
   * @return A SerializedMessage containing the serialized message.
   */
  template<typename T, std::enable_if_t<traits::IsOutMessageVariant<T>::value, int> = 0>
  static SerializedMessage serialize(const T& msg)
  {
    if constexpr(std::is_same_v<T, Success>) {
      return serializeSuccess(msg);
    } else if constexpr(std::is_same_v<T, BinaryDataBuffer>) {
      return serializeBinaryDataBuffer(msg);
    } else if constexpr(std::is_same_v<T, ImageBinaryData>) {
      return serializeImageBinaryData(msg);
    } else if constexpr(std::is_same_v<T, ColorImageBinaryData>) {
      return serializeColorImageBinaryData(msg);
    } else if constexpr(std::is_same_v<T, Error>) {
      return serializeError(msg);
    } else
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Serializer::serialize()");
  }

  static std::string serializeValue(const internal::value_t& value);
  template<typename T, std::enable_if_t<internal::traits::IsInValueTVariant<T>::value, int> = 0>
  static std::string serializeValue(const T& value)
  {
    if constexpr(std::is_same_v<T, int>) {
      return std::to_string(value);
    } else if constexpr(std::is_same_v<T, double>) {
      return std::to_string(value);
    } else if constexpr(std::is_same_v<T, std::string>) {
      return "{" + value + "}";
    } else
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Serializer::serializeValue()");
  }

private:
  Serializer() = delete;

  static SerializedMessage serializeSuccess(const Success& msg);
  static SerializedMessage serializeBinaryDataBuffer(const BinaryDataBuffer& msg);
  static SerializedMessage serializeImageBinaryData(const ImageBinaryData& msg);
  static SerializedMessage serializeColorImageBinaryData(const ColorImageBinaryData& msg);
  static SerializedMessage serializeError(const Error& msg);
};

}  // namespace protocol
}  // namespace irsol
