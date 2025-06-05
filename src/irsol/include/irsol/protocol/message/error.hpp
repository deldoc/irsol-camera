/**
 * @file irsol/protocol/message/error.hpp
 * @brief Protocol error message representation.
 *
 * This header defines the `Error` struct used to represent errors encountered during the
 * processing of incoming protocol messages. These may include syntax issues, type mismatches,
 * unsupported operations, or invalid input formats.
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

namespace irsol {
namespace protocol {

/**
 * @ingroup Protocol
 * @brief Represents an error response message from the server.
 *
 * An `Error` message is generated when a protocol input message fails to be parsed,
 * interpreted, or executed correctly. It includes a reference identifier (if available),
 * the type of message that caused the error, and a human-readable description of the issue.
 *
 * Can be stored in a @ref irsol::protocol::OutMessage variant.
 */
struct Error
{
  /// The identifier related to the failed operation (e.g., the variable or command name).
  std::string identifier;

  /// The kind of input message that triggered the error.
  InMessageKind source;

  /// A descriptive message explaining the cause of the error.
  std::string description;

  /**
   * @brief Converts the error to a human-readable string.
   * @return A string representation of the error.
   */
  std::string toString() const;

  /**
   * @brief Creates an error from a specific incoming message type.
   *
   * This templated overload supports @ref Assignment, @ref Command, and @ref Inquiry input types.
   *
   * @tparam T The incoming message type.
   * @param msg The message that caused the error.
   * @param description The explanation of the error.
   * @return An Error instance describing the issue.
   */
  template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
  static Error from(const T& msg, const std::string& description)
  {
    if constexpr(std::is_same_v<T, Assignment>) {
      return Error(msg.identifier, InMessageKind::ASSIGNMENT, description);
    } else if constexpr(std::is_same_v<T, Inquiry>) {
      return Error(msg.identifier, InMessageKind::INQUIRY, description);
    } else if constexpr(std::is_same_v<T, Command>) {
      return Error(msg.identifier, InMessageKind::COMMAND, description);
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Error::from<T>()");
    }
  }

  /**
   * @brief Creates an error from a generic InMessage variant.
   * @param message The original message variant.
   * @param description The explanation of the error.
   * @return An Error instance describing the issue.
   */
  static Error from(const InMessage& message, const std::string& description);

private:
  // Construction is only permitted through factory methods
  Error(const std::string& identifier, InMessageKind source, const std::string& description);
};

}  // namespace protocol
}  // namespace irsol
