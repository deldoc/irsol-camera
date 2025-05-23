#pragma once

#include "irsol/protocol/message/variants.hpp"
#include "irsol/protocol/message/types.hpp"

#include <string>

namespace irsol {
namespace protocol {

/**
 * @brief Represents an assignment operation in the protocol.
 *
 * An assignment consists of an identifier and a value, which can be
 * an integer, double, or string. Typically corresponds to input like `x=42`.
 */
struct Assignment
{
  Assignment(const std::string& identifier, internal::value_t value);

  /// The variable or parameter name being assigned. Must start with a character, followed by
  /// alphanumeric characters and underscores.
  const std::string identifier;

  /// The value assigned to the identifier (int, double, or string).
  const internal::value_t value;

  /**
   * @brief Converts the assignment to a human-readable string.
   * @return A string representation of the assignment (e.g., "x=42").
   */
  std::string toString() const;

  /// @return true if the value is of type int.
  bool hasInt() const;

  /// @return true if the value is of type double.
  bool hasDouble() const;

  /// @return true if the value is of type string.
  bool hasString() const;
};


}  // namespace protocol
}  // namespace irsol
