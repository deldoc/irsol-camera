/**
 * @file irsol/protocol/message/assignment.hpp
 * @brief Protocol assignment operation representation.
 *
 * This header defines the `Assignment` struct used to represent variable or parameter
 * assignments within the protocol. An assignment consists of a valid identifier and an
 * associated value (int, double, or string).
 *
 * @ingroup Protocol
 */

#pragma once

#include "irsol/protocol/message/variants.hpp"
#include "irsol/types.hpp"

#include <string>

namespace irsol {
namespace protocol {

/**
 * @ingroup Protocol
 * @brief Represents an assignment operation in the protocol.
 *
 * An assignment consists of an identifier and a value, which can be
 * an integer, double, or string. Typically corresponds to input like `x=42`.
 *
 * Can be stored in a @ref irsol::protocol::InMessage variant.
 */
struct Assignment
{
  /**
   * @brief Constructs an Assignment.
   * @param identifier The identifier string; must start with a character and contain
   *                   alphanumeric characters or underscores.
   * @param value The value to assign (int, double, or string).
   */
  Assignment(const std::string& identifier, irsol::types::protocol_value_t value);

  /// The variable or parameter name being assigned.
  std::string identifier;

  /// The value assigned to the identifier (int, double, or string).
  irsol::types::protocol_value_t value;

  /**
   * @brief Converts the assignment to a human-readable string.
   * @return A string representation of the assignment.
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
