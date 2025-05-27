#pragma once

#include "irsol/protocol/message/in_messages.hpp"
#include "irsol/protocol/message/types.hpp"
#include "irsol/protocol/message/variants.hpp"

#include <string>
#include <type_traits>

namespace irsol {
namespace protocol {

/**
 * @brief Represents a response status message from the protocol.
 *
 * A status may include an optional body with additional information.
 */
struct Success
{

  /// The identifier associated with the status. Must start with a character, followed by
  /// alphanumeric characters and underscores.
  const std::string identifier;

  /// The kind of the incoming message that generated this status.
  const InMessageKind source;

  /// Optional body providing further detail about the status. Only used when the source is INQUIRY
  /// or ASSIGNMENT.
  const std::optional<internal::value_t> body{};

  /**
   * @brief Converts the status to a human-readable string.
   * @return A string representation of the status message.
   */
  std::string toString() const;

  /// @return true if a body is present in the status.
  bool hasBody() const;

  /// @return true if the value is of type int.
  bool hasInt() const;

  /// @return true if the value is of type double.
  bool hasDouble() const;

  /// @return true if the value is of type string.
  bool hasString() const;

  template<
    typename T,
    std::enable_if_t<
      traits::IsInMessageVariant<std::decay_t<T>>::value &&
        !std::is_same_v<std::decay_t<T>, Inquiry>,
      int> = 0>
  static Success from(T&& msg)
  {
    using U = std::decay_t<T>;
    if constexpr(std::is_same_v<U, Assignment>) {
      return Success(msg.identifier, InMessageKind::ASSIGNMENT, msg.value);
    } else if constexpr(std::is_same_v<U, Command>) {
      return Success(msg.identifier, InMessageKind::COMMAND);
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(U, "Success::from<T>()");
    }
  }

  static Success from(const Inquiry& msg, internal::value_t result)
  {
    return Success(msg.identifier, InMessageKind::INQUIRY, std::make_optional(result));
  }

private:
  // Only allow construction from factory-methods
  Success(
    const std::string&               identifier,
    InMessageKind                    source,
    std::optional<internal::value_t> body = std::nullopt);
};

}  // namespace protocol
}  // namespace irsol
