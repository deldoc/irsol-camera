#pragma once

#include "irsol/macros.hpp"
#include "irsol/protocol/message.hpp"
#include "irsol/protocol/serialization/serialized_message.hpp"
#include "irsol/traits.hpp"

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
  /**
   * @brief Utility enumerators defining special bytes used by the Simple Communication Protocol.
   */
  struct SpecialBytes
  {
    static constexpr irsol::types::byte_t SOH{0x01};
    static constexpr irsol::types::byte_t STX{0x02};
    static constexpr irsol::types::byte_t ETX{0x03};
  };

  static constexpr const char* message_termination = "\n";
  /*
   * Serialize an OutMessage into a string.
   *
   * @param msg The message to serialize.
   * @return A SerializedMessage containing the serialized message.
   */
  static internal::SerializedMessage serialize(OutMessage&& msg);

  /**
   * Serialize a specific OutMessage type into a string.
   *
   * @param msg The message to serialize. Must be a class used in the OutMessage variant.
   * @return A SerializedMessage containing the serialized message.
   */
  template<
    typename T,
    std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
  static internal::SerializedMessage serialize(T&& msg)
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
  template<
    typename T,
    std::enable_if_t<
      irsol::traits::is_type_in_variant<T, irsol::types::protocol_value_t>::value,
      int> = 0>
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

  static std::string serializeBinaryDataAttribute(irsol::protocol::BinaryDataAttribute&& att);

private:
  Serializer() = delete;

  static internal::SerializedMessage serializeSuccess(Success&& msg);
  static internal::SerializedMessage serializeBinaryDataBuffer(BinaryDataBuffer&& msg);
  static internal::SerializedMessage serializeImageBinaryData(ImageBinaryData&& msg);
  static internal::SerializedMessage serializeColorImageBinaryData(ColorImageBinaryData&& msg);
  static internal::SerializedMessage serializeError(Error&& msg);
};

}  // namespace protocol
}  // namespace irsol
