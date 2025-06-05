/**
 * @file irsol/protocol/message/command.hpp
 * @brief Protocol command representation.
 *
 * This header defines the `Command` struct used to represent command invocations
 * within the protocol. Commands typically consist of a valid identifier and
 * do not carry any associated value, serving instead to trigger a predefined
 * action or behavior.
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
 * @brief Represents a command invocation in the protocol.
 *
 * A command consists of a valid identifier and typically does not carry a value.
 * It is used to trigger predefined actions such as "start", "stop", or "reset".
 *
 * Can be stored in a @ref irsol::protocol::InMessage variant.
 */
struct Command
{
  /**
   * @brief Constructs a Command.
   * @param identifier The command name; must start with a character and contain
   *                   alphanumeric characters or underscores.
   */
  Command(const std::string& identifier);

  /// The name of the command to execute.
  std::string identifier;

  /**
   * @brief Converts the command to a human-readable string.
   * @return A string representation of the command.
   */
  std::string toString() const;
};

}  // namespace protocol
}  // namespace irsol
