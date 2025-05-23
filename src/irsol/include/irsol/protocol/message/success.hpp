#pragma once

#include "irsol/protocol/message/variants.hpp"
#include "irsol/protocol/message/types.hpp"
#include "irsol/protocol/message/in_messages.hpp"

#include <string>

namespace irsol {
namespace protocol {

/**
 * @brief Represents a response status message from the protocol.
 *
 * A status may include an optional body with additional information.
 */
struct Success
{
  Success(const std::string& identifier, std::optional<internal::value_t> body = std::nullopt);

  /// The identifier associated with the status. Must start with a character, followed by
  /// alphanumeric characters and underscores.
  const std::string identifier;

  /// Optional body providing further detail about the status.
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

  template<typename T, std::enable_if_t<traits::IsInMessageVariant<T>::value, int> = 0>
  static Success from(const T& msg)
  {
    if constexpr(std::is_same_v<T, Assignment>) {
      return Success(msg.identifier, msg.value);
    } else if constexpr(std::is_same_v<T, Inquiry>) {
      return Success(msg.identifier);
    } else if constexpr(std::is_same_v<T, Command>) {
      return Success(msg.identifier);
    } else {
      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "Success::from<T>()");
    }
  }

  static Success from(const InMessage& msg);
};


}  // namespace protocol
}  // namespace irsol
