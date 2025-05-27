#pragma once

#include "irsol/protocol/message/in_messages.hpp"
#include "irsol/protocol/message/types.hpp"
#include "irsol/protocol/message/variants.hpp"

#include <string>

namespace irsol {
namespace protocol {

/**
 * @brief Represents an error message in the protocol.
 *
 * Used to convey problems such as parsing failures or invalid commands.
 */
struct Error
{
  /// Identifier related to the error (e.g., the failed command). Must start with a character,
  /// followed by alphanumeric characters and underscores.
  const std::string identifier;

  /// The kind of the incoming message that generated this error.
  const InMessageKind source;

  /// Human-readable error description.
  const std::string description;

  /**
   * @brief Converts the error to a human-readable string.
   * @return A string representation of the error message.
   */
  std::string toString() const;

  template<typename T, std::enable_if_t<traits::IsInMessageVariant<T>::value, int> = 0>
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

  static Error from(const InMessage& message, const std::string& description);

private:
  // Only allow construction from factory-methods
  Error(const std::string& identifier, InMessageKind source, const std::string& description);
};

}  // namespace protocol
}  // namespace irsol
