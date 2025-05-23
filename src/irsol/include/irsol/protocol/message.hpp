#pragma once

#include "irsol/protocol/binary_message.hpp"
#include "irsol/protocol/types.hpp"

#include <array>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

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
  Inquiry(const std::string& identifier);

  /// The identifier whose value is being requested. Must start with a character, followed by
  /// alphanumeric characters and underscores.
  const std::string identifier;

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
  Command(const std::string& identifier);

  /// The name of the command to execute. Must start with a character, followed by alphanumeric
  /// characters and underscores.
  const std::string identifier;

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
  Status(const std::string& identifier, std::optional<std::string> body = std::nullopt);

  /// The identifier associated with the status. Must start with a character, followed by
  /// alphanumeric characters and underscores.
  const std::string identifier;

  /// Optional body providing further detail about the status.
  const std::optional<std::string> body{};

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
  Error(const std::string& identifier, const std::string& description);

  /// Identifier related to the error (e.g., the failed command). Must start with a character,
  /// followed by alphanumeric characters and underscores.
  const std::string identifier;

  /// Human-readable error description.
  const std::string description;

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
 * @brief Represents the type of an outgoing message.
 */
enum class OutMessageKind
{
  STATUS,
  BINARY_BUFFER,
  BW_IMAGE,
  COLOR_IMAGE,
  ERROR
};

/**
 * @brief Represents any outgoing message type sent in response.
 *
 * This includes status messages and errors.
 */
using OutMessage =
  std::variant<Status, BinaryDataBuffer, ImageBinaryData, ColorImageBinaryData, Error>;

namespace internal {
// Create trait helpers to limit the compilation possibility of some template functions
// to only types that are part of the InMessage/OutMessage variants.

template<typename T, typename Variant>
struct _IsTypeInVariant;

template<typename T, typename... Ts>
struct _IsTypeInVariant<T, std::variant<Ts...>> : std::disjunction<std::is_same<T, Ts>...>
{};

// To be used as:
// template <typename T, std::enable_if_t<internal::IsInMessageVariant<T>::value, int> = 0>
// void myFunctionTemplate(const T& msg) { ... }
// in this way, the 'myFunctionTemplate' will only accept types that are part of the InMessage
// variant. and if used with a type that is not part of the InMessage variant, the compiler will
// issue a compilation error.
template<typename T>
using IsInMessageVariant = _IsTypeInVariant<T, InMessage>;

// To be used as:
// template <typename T, std::enable_if_t<internal::IsOutMessageVariant<T>::value, int> = 0>
// void myFunctionTemplate(const T& msg) { ... }
// in this way, the 'myFunctionTemplate' will only accept types that are part of the OutMessage
// variant. and if used with a type that is not part of the OutMessage variant, the compiler will
// issue a compilation error.
template<typename T>
using IsOutMessageVariant = _IsTypeInVariant<T, OutMessage>;
}  // namespace internal

template<typename T, std::enable_if_t<internal::IsInMessageVariant<T>::value, int> = 0>
constexpr InMessageKind
getInMessageKind(const T&)
{
  if constexpr(std::is_same_v<T, Assignment>)
    return InMessageKind::ASSIGNMENT;
  else if constexpr(std::is_same_v<T, Inquiry>)
    return InMessageKind::INQUIRY;
  else if constexpr(std::is_same_v<T, Command>)
    return InMessageKind::COMMAND;
  else
    static_assert(
      std::is_same_v<T, void>, "Unsupported InMessage type");  // This line should never be reached.
}

/**
 * @brief Returns the kind of the given InMessage.
 * @param msg The incoming message.
 * @return The InMessageKind enum value representing the actual type.
 */
InMessageKind getInMessageKind(const InMessage& msg);

template<typename T, std::enable_if_t<internal::IsInMessageVariant<T>::value, int> = 0>
constexpr bool
isAssignment(const T&)
{
  return std::is_same_v<T, Assignment>;
}
bool isAssignment(const InMessage& msg);

template<typename T, std::enable_if_t<internal::IsInMessageVariant<T>::value, int> = 0>
constexpr bool
isInquiry(const T&)
{
  return std::is_same_v<T, Inquiry>;
}
bool isInquiry(const InMessage& msg);

template<typename T, std::enable_if_t<internal::IsInMessageVariant<T>::value, int> = 0>
constexpr bool
isCommand(const T&)
{
  return std::is_same_v<T, Command>;
}
bool isCommand(const InMessage& msg);

template<typename T, std::enable_if_t<internal::IsOutMessageVariant<T>::value, int> = 0>
constexpr OutMessageKind
getOutMessageKind(const T&)
{
  if constexpr(std::is_same_v<T, Status>)
    return OutMessageKind::STATUS;
  else if constexpr(std::is_same_v<T, Error>)
    return OutMessageKind::ERROR;
  else if constexpr(std::is_same_v<T, BinaryDataBuffer>)
    return OutMessageKind::BINARY_BUFFER;
  else if constexpr(std::is_same_v<T, ImageBinaryData>)
    return OutMessageKind::BW_IMAGE;
  else if constexpr(std::is_same_v<T, ColorImageBinaryData>)
    return OutMessageKind::COLOR_IMAGE;
  else
    static_assert(
      std::is_same_v<T, void>,
      "Unsupported OutMessage type");  // This line should never be reached.
}
/**
 * @brief Returns the kind of the given OutMessage.
 * @param msg The outgoing message.
 * @return The OutMessageKind enum value representing the actual type.
 */
OutMessageKind getOutMessageKind(const OutMessage& msg);

template<typename T, std::enable_if_t<internal::IsOutMessageVariant<T>::value, int> = 0>
constexpr bool
isStatus(const T&)
{
  return std::is_same_v<T, Status>;
}
bool isStatus(const OutMessage& msg);

template<typename T, std::enable_if_t<internal::IsOutMessageVariant<T>::value, int> = 0>
constexpr bool
isBinaryDataBuffer(const T&)
{
  return std::is_same_v<T, BinaryDataBuffer>;
}
bool isBinaryDataBuffer(const OutMessage& msg);

template<typename T, std::enable_if_t<internal::IsOutMessageVariant<T>::value, int> = 0>
constexpr bool
isImageBinaryData(const T&)
{
  return std::is_same_v<T, ImageBinaryData>;
}
bool isImageBinaryData(const OutMessage& msg);

template<typename T, std::enable_if_t<internal::IsOutMessageVariant<T>::value, int> = 0>
constexpr bool
isColorImageBinaryData(const T&)
{
  return std::is_same_v<T, ColorImageBinaryData>;
}
bool isColorImageBinaryData(const OutMessage& msg);

template<typename T, std::enable_if_t<internal::IsOutMessageVariant<T>::value, int> = 0>
constexpr bool
isError(const T&)
{
  return std::is_same_v<T, Error>;
}
bool isError(const OutMessage& msg);

}  // namespace protocol
}  // namespace irsol
