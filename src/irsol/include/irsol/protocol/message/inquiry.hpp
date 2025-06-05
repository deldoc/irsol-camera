/**
 * @file irsol/protocol/message/inquiry.hpp
 * @brief Protocol inquiry representation.
 *
 * This header defines the `Inquiry` struct used to represent value lookups
 * within the protocol. An inquiry consists of a valid identifier and is typically
 * used to query the current value of a variable or parameter (e.g., `x?`).
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
 * @brief Represents a value inquiry in the protocol.
 *
 * An inquiry consists of an identifier and is used to request the current value
 * associated with that identifier. Typically corresponds to input like `x?`.
 *
 * Can be stored in a @ref irsol::protocol::InMessage variant.
 */
struct Inquiry
{
  /**
   * @brief Constructs an Inquiry.
   * @param identifier The identifier string; must start with a character and contain
   *                   alphanumeric characters or underscores.
   */
  Inquiry(const std::string& identifier);

  /// The identifier whose value is being requested.
  std::string identifier;

  /**
   * @brief Converts the inquiry to a human-readable string.
   * @return A string representation of the inquiry.
   */
  std::string toString() const;
};

}  // namespace protocol
}  // namespace irsol
