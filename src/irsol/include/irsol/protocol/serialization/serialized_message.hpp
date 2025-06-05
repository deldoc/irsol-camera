/**
 * @file irsol/protocol/serialization/serialized_message.hpp
 * @brief Defines the SerializedMessage structure representing serialized protocol messages.
 *
 * This file declares the @ref irsol::protocol::internal::SerializedMessage struct,
 * which encapsulates a serialized protocol message composed of a textual header and a binary
 * payload.
 *
 * The structure provides move semantics for efficient handling and utility methods to query
 * the presence and size of its components, as well as to convert the serialized message into a
 * string.
 *
 * @ingroup Protocol
 */

#pragma once

#include "irsol/types.hpp"

#include <string>
#include <vector>

namespace irsol {
namespace protocol {
namespace internal {

/**
 * @ingroup Protocol
 * @brief Represents a serialized protocol message with header and payload.
 *
 * This structure stores the serialized form of an outgoing protocol message.
 * It consists of:
 * - A textual header stored as a `std::string`.
 * - A binary payload stored as a `std::vector<irsol::types::byte_t>`.
 *
 * The class supports move semantics but disables copying to avoid expensive copies of potentially
 * large payloads.
 *
 * Utility member functions provide information about the presence of header and payload data,
 * the payload size, and allow conversion to a string representation.
 */
struct SerializedMessage
{
  /**
   * @brief The textual header of the serialized message.
   *
   * This string typically contains the message type and metadata in a human-readable format.
   */
  std::string header;

  /**
   * @brief The binary payload of the serialized message.
   *
   * This vector contains the binary data part of the message, following the header.
   */
  std::vector<irsol::types::byte_t> payload{};

  /**
   * @brief Constructs a SerializedMessage with a header and binary payload.
   *
   * The constructor moves the payload into the member to avoid copies.
   *
   * @param header The message header string.
   * @param payload The binary payload vector to move.
   */
  SerializedMessage(const std::string& header, std::vector<irsol::types::byte_t>&& payload);

  /// Move constructor (defaulted).
  SerializedMessage(SerializedMessage&&) noexcept = default;

  /// Move assignment operator (defaulted).
  SerializedMessage& operator=(SerializedMessage&&) noexcept = default;

  /// Deleted copy constructor to prevent copying.
  SerializedMessage(const SerializedMessage&) = delete;

  /// Deleted copy assignment operator to prevent copying.
  SerializedMessage& operator=(const SerializedMessage&) = delete;

  /**
   * @brief Checks whether the serialized message contains a non-empty header.
   * @return `true` if the header string is not empty, `false` otherwise.
   */
  bool hasHeader() const;

  /**
   * @brief Checks whether the serialized message contains a non-empty payload.
   * @return `true` if the payload vector is not empty, `false` otherwise.
   */
  bool hasPayload() const;

  /**
   * @brief Returns the size of the payload in bytes.
   * @return The number of bytes stored in the payload.
   */
  size_t payloadSize() const;

  /**
   * @brief Converts the serialized message to a string representation.
   *
   * This method typically returns a human-readable form of the serialized message,
   * possibly concatenating the header and a textual form of the payload.
   *
   * @return A string representation of the serialized message.
   */
  std::string toString() const;
};

}  // namespace internal
}  // namespace protocol
}  // namespace irsol
