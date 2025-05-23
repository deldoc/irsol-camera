#pragma once
#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "irsol/protocol/message.hpp"
#include "irsol/protocol/types.hpp"
#include "irsol/utils.hpp"

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
    if constexpr(std::is_same_v<T, Status>) {
      IRSOL_LOG_INFO("Serializing status message: {}", msg.toString());
      return "test" + msg.toString();
    } else if constexpr(std::is_same_v<T, BinaryDataBuffer>) {
      IRSOL_LOG_INFO("Serializing binary buffer: {}", msg.toString());
      return "test" + msg.toString();
    } else if constexpr(std::is_same_v<T, ImageBinaryData>) {
      IRSOL_LOG_INFO("Serializing image binary data: {}", msg.toString());
      return "test" + msg.toString();
    } else if constexpr(std::is_same_v<T, ColorImageBinaryData>) {
      IRSOL_LOG_INFO("Serializing color image binary data: {}", msg.toString());
      return "test" + msg.toString();
    } else if constexpr(std::is_same_v<T, Error>) {
      IRSOL_LOG_INFO("Serializing error message: {}", msg.toString());
      return "test" + msg.toString();
    } else
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Serializer::serialize()");
  }

private:
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
};

}  // namespace protocol
}  // namespace irsol
