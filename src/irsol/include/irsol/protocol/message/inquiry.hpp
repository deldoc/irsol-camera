#pragma once

#include "irsol/protocol/message/variants.hpp"
#include "irsol/types.hpp"

#include <string>

namespace irsol {
namespace protocol {

/**
 * @brief Represents a value inquiry for a specific identifier.
 *
 * Typically corresponds to input like `x?` to query the current value of `x`.
 */
struct Inquiry
{
  Inquiry(const std::string& identifier);

  /// The identifier whose value is being requested. Must start with a character, followed by
  /// alphanumeric characters and underscores.
  std::string identifier;

  /**
   * @brief Converts the inquiry to a human-readable string.
   * @return A string representation of the inquiry (e.g., "x?").
   */
  std::string toString() const;
};

}  // namespace protocol
}  // namespace irsol
