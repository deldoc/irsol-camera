#pragma once

#include "irsol/assert.hpp"
#include "irsol/protocol/message.hpp"

#include <string>

namespace irsol {
namespace protocol {

class Serializer
{
public:
  /*
   * Serialize an OutMessage into a string.
   *
   * @param msg The message to serialize.
   * @return A string containing the serialized message.
   */
  static std::string serialize(const OutMessage& msg);

  /**
   * Serialize a specific OutMessage type into a string.
   *
   * @param msg The message to serialize. Must be a class used in the OutMessage variant.
   * @return A string containing the serialized message.
   */
  template<typename T, std::enable_if_t<traits::IsOutMessageVariant<T>::value, int> = 0>
  static std::string serialize(const T& msg)
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

  static std::string serializeSuccess(const Success& msg);
  static std::string serializeBinaryDataBuffer(const BinaryDataBuffer& msg);
  static std::string serializeImageBinaryData(const ImageBinaryData& msg);
  static std::string serializeColorImageBinaryData(const ColorImageBinaryData& msg);
  static std::string serializeError(const Error& msg);
};

}  // namespace protocol
}  // namespace irsol
