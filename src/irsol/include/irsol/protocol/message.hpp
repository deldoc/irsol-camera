#pragma once

#include "irsol/protocol/types.hpp"

#include <optional>
#include <string>
#include <variant>

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
  /// The variable or parameter name being assigned.
  std::string identifier;

  /// The value assigned to the identifier (int, double, or string).
  internal::value_t value;

  /**
   * @brief Converts the assignment to a protocol-formatted string.
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

/**
 * @brief Represents a value inquiry for a specific identifier.
 *
 * Typically corresponds to input like `x?` to query the current value of `x`.
 */
struct Inquiry
{
  /// The identifier whose value is being requested.
  std::string identifier;

  /**
   * @brief Converts the inquiry to a protocol-formatted string.
   * @return A string representation of the inquiry (e.g., "x?").
   */
  std::string toString() const;
};

/**
 * @brief Represents a command invocation in the protocol.
 *
 * A command typically has no associated value but may trigger a predefined action.
 */
struct Command
{
  /// The name of the command to execute.
  std::string identifier;

  /**
   * @brief Converts the command to a protocol-formatted string.
   * @return A string representation of the command (e.g., "reset").
   */
  std::string toString() const;
};

/**
 * @brief Represents a response status message from the protocol.
 *
 * A status may include an optional body with additional information.
 */
struct Status
{
  /// The identifier associated with the status.
  std::string identifier;

  /// Optional body providing further detail about the status.
  std::optional<std::string> body{};

  /**
   * @brief Converts the status to a protocol-formatted string.
   * @return A string representation of the status message.
   */
  std::string toString() const;

  /// @return true if a body is present in the status.
  bool hasBody() const;
};

/**
 * @brief Represents an error message in the protocol.
 *
 * Used to convey problems such as parsing failures or invalid commands.
 */
struct Error
{
  /// Identifier related to the error (e.g., the failed command).
  std::string identifier;

  /// Human-readable error description.
  std::string description;

  /**
   * @brief Converts the error to a protocol-formatted string.
   * @return A string representation of the error message.
   */
  std::string toString() const;
};

/**
 * @brief Represents the type of an incoming message.
 */
enum class InMessageKind
{
  ASSIGNMENT,
  INQUIRY,
  COMMAND
};

/**
 * @brief Represents any incoming message type that can be parsed.
 *
 * This includes assignments, inquiries, and commands.
 */
using InMessage = std::variant<Assignment, Inquiry, Command>;

/**
 * @brief Returns the kind of the given InMessage.
 * @param msg The incoming message.
 * @return The InMessageKind enum value representing the actual type.
 */
InMessageKind getMessageKind(const InMessage& msg);

/**
 * @brief Represents the type of an outgoing message.
 */
enum class OutMessageKind
{
  STATUS,
  ERROR
};

/**
 * @brief Represents any outgoing message type sent in response.
 *
 * This includes status messages and errors.
 */
using OutMessage = std::variant<Status, Error>;

/**
 * @brief Returns the kind of the given OutMessage.
 * @param msg The outgoing message.
 * @return The OutMessageKind enum value representing the actual type.
 */
OutMessageKind getMessageKind(const OutMessage& msg);

bool isAssignment(const InMessage& msg);

bool isInquiry(const InMessage& msg);

bool isCommand(const InMessage& msg);

bool isStatus(const OutMessage& msg);

bool isError(const OutMessage& msg);

}  // namespace protocol
}  // namespace irsol
