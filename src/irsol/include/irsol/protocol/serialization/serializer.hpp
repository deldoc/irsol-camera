#pragma once

#include "irsol/assert.hpp"
#include "irsol/protocol/message.hpp"
#include "irsol/protocol/serialization/serialized_message.hpp"

#include <string>
#include <vector>

namespace irsol {
namespace protocol {

/**
 * A class for serializing OutMessage and primitive types into SerializedMessage instances.
 *
 * This class provides a generic serialization interface for OutMessage types and primitive types.
 * The class **consumes** the provided message: after serialization the original message is invalid.
 */
class Serializer
{
public:
  /*
   * Serialize an OutMessage into a string.
   *
   * @param msg The message to serialize.
   * @return A SerializedMessage containing the serialized message.
   */
  static SerializedMessage serialize(OutMessage&& msg);

  /**
   * Serialize a specific OutMessage type into a string.
   *
   * @param msg The message to serialize. Must be a class used in the OutMessage variant.
   * @return A SerializedMessage containing the serialized message.
   */
  template<typename T, std::enable_if_t<traits::IsOutMessageVariant<T>::value, int> = 0>
  static SerializedMessage serialize(T&& msg)
  {
    if constexpr(std::is_same_v<T, Success>) {
      return serializeSuccess(std::move(msg));
    } else if constexpr(std::is_same_v<T, BinaryDataBuffer>) {
      return serializeBinaryDataBuffer(std::move(msg));
    } else if constexpr(std::is_same_v<T, ImageBinaryData>) {
      return serializeImageBinaryData(std::move(msg));
    } else if constexpr(std::is_same_v<T, ColorImageBinaryData>) {
      return serializeColorImageBinaryData(std::move(msg));
    } else if constexpr(std::is_same_v<T, Error>) {
      return serializeError(std::move(msg));
    } else
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Serializer::serialize()");
  }

  static std::string serializeValue(irsol::types::protocol_value_t&& value);
  template<typename T, std::enable_if_t<internal::traits::IsInValueTVariant<T>::value, int> = 0>
  static std::string serializeValue(T&& value)
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

  static SerializedMessage serializeSuccess(Success&& msg);
  static SerializedMessage serializeBinaryDataBuffer(BinaryDataBuffer&& msg);
  static SerializedMessage serializeImageBinaryData(ImageBinaryData&& msg);
  static SerializedMessage serializeColorImageBinaryData(ColorImageBinaryData&& msg);
  static SerializedMessage serializeError(Error&& msg);
};

}  // namespace protocol
}  // namespace irsol
