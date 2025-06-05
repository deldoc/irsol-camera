/**
 * @file irsol/protocol/serialization/serializer.hpp
 * @brief Serialization utilities for protocol messages and primitive values.
 *
 * This file defines the `Serializer` class that provides static methods to convert
 * protocol outgoing messages (@ref irsol::protocol::OutMessage variants) and primitive protocol
 * values into serialized string representations (@ref
 * irsol::protocol::internal::SerializedMessage).
 *
 * The serialization format follows the Simple Communication Protocol used in irsol,
 * with special byte markers and terminators as defined in this class.
 *
 * @ingroup Protocol
 */

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
 * @ingroup Protocol
 * @brief Serializes outgoing protocol messages and primitive values.
 *
 * This class offers a collection of static methods to serialize @ref irsol::protocol::OutMessage
 * variant types, as well as protocol primitive values such as integers, doubles, and strings
 * (@ref irsol::types::protocol_value_t), into @ref irsol::protocol::internal::SerializedMessage
 * instances or plain strings.
 *
 * The class uses compile-time checks to ensure that only valid message types are serialized.
 * It also provides utility constants for the special byte markers used in the protocol,
 * following the Simple Communication Protocol specification used by irsol.
 *
 * The serialization consumes the input message or value (via rvalue references),
 * leaving the original object in a valid but unspecified state.
 */
class Serializer
{
public:
  /**
   * @brief Special byte constants used in the Simple Communication Protocol.
   *
   * These bytes mark the start of header, start of text, and end of text in binary protocol
   * messages.
   */
  struct SpecialBytes
  {
    /// Start of Header (SOH) byte: 0x01
    static constexpr irsol::types::byte_t SOH{0x01};
    /// Start of Text (STX) byte: 0x02
    static constexpr irsol::types::byte_t STX{0x02};
    /// End of Text (ETX) byte: 0x03
    static constexpr irsol::types::byte_t ETX{0x03};
  };

  /// Message line termination sequence (newline character) used to "close" a serialized message.
  static constexpr const char* message_termination = "\n";

  /**
   * @brief Serialize an @ref irsol::protocol::OutMessage variant into a serialized protocol
   * message.
   *
   * The message is consumed by this operation.
   *
   * @param msg The @ref irsol::protocol::OutMessage to serialize.
   * @return A @ref irsol::protocol::internal::SerializedMessage containing the serialized message.
   */
  static internal::SerializedMessage serialize(OutMessage&& msg);

  /**
   * @brief Serialize a specific @ref irsol::protocol::OutMessage type into a serialized protocol
   * message.
   *
   * Enabled only for types contained in the @ref irsol::protocol::OutMessage variant.
   * The method dispatches to specialized serialization functions based on message type.
   *
   * @tparam T The specific @ref irsol::protocol::OutMessage type to serialize.
   * @param msg The message to serialize.
   * @return A @ref irsol::protocol::internal::SerializedMessage containing the serialized message.
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

  /**
   * @brief Serialize a protocol primitive value to a string.
   *
   * Supported types include `int`, `double`, and `std::string` (see @ref
   * irsol::types::protocol_value_t).
   *
   * @param value The protocol value to serialize.
   * @return A string representation of the serialized value.
   */
  static std::string serializeValue(irsol::types::protocol_value_t&& value);

  /**
   * @brief Serialize a specific primitive type to a string.
   *
   * Enabled only for types contained in irsol::types::protocol_value_t.
   *
   * @tparam T The primitive type to serialize (`int`, `double`, or `std::string`).
   * @param value The value to serialize.
   * @return A string representation of the serialized value.
   */
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

  /**
   * @brief Serialize a @ref irsol::protocol::BinaryDataAttribute to a string.
   *
   * @param att The @ref irsol::protocol::BinaryDataAttribute to serialize.
   * @return A string representation of the serialized attribute.
   */
  static std::string serializeBinaryDataAttribute(irsol::protocol::BinaryDataAttribute&& att);

private:
  Serializer() = delete;  ///< This class cannot be instantiated.

  // Specialized serialization methods for each OutMessage type:

  static internal::SerializedMessage serializeSuccess(Success&& msg);
  static internal::SerializedMessage serializeBinaryDataBuffer(BinaryDataBuffer&& msg);
  static internal::SerializedMessage serializeImageBinaryData(ImageBinaryData&& msg);
  static internal::SerializedMessage serializeColorImageBinaryData(ColorImageBinaryData&& msg);
  static internal::SerializedMessage serializeError(Error&& msg);
};

}  // namespace protocol
}  // namespace irsol
