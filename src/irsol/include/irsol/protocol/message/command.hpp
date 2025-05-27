#pragma once

#include "irsol/protocol/message/variants.hpp"
#include "irsol/types.hpp"

#include <string>

namespace irsol {
namespace protocol {

/**
 * @brief Represents a command invocation in the protocol.
 *
 * A command typically has no associated value but may trigger a predefined action.
 */
struct Command
{
  Command(const std::string& identifier);

  /// The name of the command to execute. Must start with a character, followed by alphanumeric
  /// characters and underscores.
  std::string identifier;

  /**
   * @brief Converts the command to a human-readable string.
   * @return A string representation of the command (e.g., "reset").
   */
  std::string toString() const;
};

}  // namespace protocol
}  // namespace irsol
