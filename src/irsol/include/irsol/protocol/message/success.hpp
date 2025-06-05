/**
 * @file irsol/protocol/message/success.hpp
 * @brief Protocol success message representation.
 *
 * This header defines the `Success` struct used to represent responses returned by the server
 * upon successful handling of protocol input messages such as assignments, commands, or inquiries.
 * A success message may optionally include a value, typically in response to an inquiry or
 * assignment.
 *
 * @ingroup Protocol
 */

#pragma once

#include "irsol/macros.hpp"
#include "irsol/protocol/message/in_messages.hpp"
#include "irsol/protocol/message/variants.hpp"
#include "irsol/traits.hpp"
#include "irsol/types.hpp"

#include <string>
#include <type_traits>

namespace irsol {
namespace protocol {

/**
 * @ingroup Protocol
 * @brief Represents a success response message from the server.
 *
 * A `Success` message acknowledges the successful processing of an incoming message (such as
 * an @ref irsol::protocol::Assignment, @ref irsol::protocol::Command, or @ref
 * irsol::protocol::Inquiry). It contains the originating identifier, the kind of message that
 * triggered the response, and optionally a result value (e.g., for assignments and inquiries).
 *
 * Can be stored in a @ref irsol::protocol::OutMessage variant.
 */
struct Success
{
  /// The identifier associated with the success response.
  std::string identifier;

  /// The kind of the incoming message that triggered this response.
  InMessageKind source;

  /// Optional result or data associated with the response.
  std::optional<irsol::types::protocol_value_t> body{};

  /**
   * @brief Converts the success message to a human-readable string.
   * @return A string representation of the success.
   */
  std::string toString() const;

  /// @return true if the success message contains a result body.
  bool hasBody() const;

  /// @return true if the result body is of type int.
  bool hasInt() const;

  /// @return true if the result body is of type double.
  bool hasDouble() const;

  /// @return true if the result body is of type string.
  bool hasString() const;

  /**
   * @brief Creates a success message from an Assignment.
   * @param msg The original assignment message.
   * @param overrideValue Optional value to return instead of the original. This is sometimes set by
   * the server, when the original value of the @ref Assignment message was not suitable for storage
   * (e.g. due to precision limitations or min/max constraints).
   * @return A success response based on the assignment.
   */
  static Success from(
    const Assignment&                             msg,
    std::optional<irsol::types::protocol_value_t> overrideValue = std::nullopt)
  {
    return Success(
      msg.identifier,
      InMessageKind::ASSIGNMENT,
      overrideValue.has_value() ? overrideValue : std::make_optional(msg.value));
  }

  /**
   * @brief Creates a success message from a Command.
   * @param msg The original command message.
   * @return A success response based on the command.
   */
  static Success from(const Command& msg)
  {
    return Success(msg.identifier, InMessageKind::COMMAND);
  }

  /**
   * @brief Creates a success message from an Inquiry.
   * @param msg The original inquiry message.
   * @param result The result of the inquiry.
   * @return A success response with the requested value.
   */
  static Success from(const Inquiry& msg, irsol::types::protocol_value_t result)
  {
    return Success(msg.identifier, InMessageKind::INQUIRY, std::make_optional(result));
  }

  /**
   * @brief Creates a standalone status message with a value.
   *
   * Useful in cases where the server responds without having access
   * to the original incoming message (e.g., asynchronous events).
   *
   * @param identifier The identifier associated with the result.
   * @param value The value to return.
   * @return A success response with the given identifier and value.
   */
  static Success asStatus(const std::string& identifier, irsol::types::protocol_value_t value)
  {
    return Success(identifier, InMessageKind::INQUIRY, std::make_optional(value));
  }

private:
  // Only allow construction through factory methods
  Success(
    const std::string&                            identifier,
    InMessageKind                                 source,
    std::optional<irsol::types::protocol_value_t> body = std::nullopt);
};

}  // namespace protocol
}  // namespace irsol
