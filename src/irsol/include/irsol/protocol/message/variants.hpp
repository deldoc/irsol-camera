#pragma once

#include "irsol/macros.hpp"
#include "irsol/protocol/message/binary.hpp"
#include "irsol/traits.hpp"
#include "irsol/types.hpp"

#include <string>
#include <variant>

namespace irsol {
namespace protocol {

/**
 * @brief Represents the type of an incoming message.
 */
enum class InMessageKind
{
  ASSIGNMENT,
  INQUIRY,
  COMMAND
};

constexpr const char*
InMessageKindToString(InMessageKind kind)
{
  switch(kind) {
    case InMessageKind::ASSIGNMENT:
      return "ASSIGNMENT";
    case InMessageKind::INQUIRY:
      return "INQUIRY";
    case InMessageKind::COMMAND:
      return "COMMAND";
  }
  IRSOL_UNREACHABLE("Invalid InMessageKind");
}

// Forward declarations
struct Assignment;
struct Inquiry;
struct Command;

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

constexpr const char*
OutMessageKindToString(OutMessageKind kind)
{
  switch(kind) {
    case OutMessageKind::STATUS:
      return "STATUS";
    case OutMessageKind::BINARY_BUFFER:
      return "BINARY_BUFFER";
    case OutMessageKind::BW_IMAGE:
      return "BW_IMAGE";
    case OutMessageKind::COLOR_IMAGE:
      return "COLOR_IMAGE";
    case OutMessageKind::ERROR:
      return "ERROR";
  }
  IRSOL_UNREACHABLE("Invalid OutMessageKind");
}

// Forward declarations
struct Success;
struct Error;

/**
 * @brief Represents any outgoing message type sent in response.
 *
 * This includes status messages and errors.
 */
using OutMessage =
  std::variant<Success, BinaryDataBuffer, ImageBinaryData, ColorImageBinaryData, Error>;

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
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
    IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "getInMessageKind()");
}

/**
 * @brief Returns the kind of the given InMessage.
 * @param msg The incoming message.
 * @return The InMessageKind enum value representing the actual type.
 */
InMessageKind getInMessageKind(const InMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
constexpr bool
isAssignment(const T&)
{
  return std::is_same_v<T, Assignment>;
}
bool isAssignment(const InMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
constexpr bool
isInquiry(const T&)
{
  return std::is_same_v<T, Inquiry>;
}
bool isInquiry(const InMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
constexpr bool
isCommand(const T&)
{
  return std::is_same_v<T, Command>;
}
bool isCommand(const InMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr OutMessageKind
getOutMessageKind(const T&)
{
  if constexpr(std::is_same_v<T, Success>)
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
    IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "getOutMessageKind()");
}
/**
 * @brief Returns the kind of the given OutMessage.
 * @param msg The outgoing message.
 * @return The OutMessageKind enum value representing the actual type.
 */
OutMessageKind getOutMessageKind(const OutMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isSuccess(const T&)
{
  return std::is_same_v<T, Success>;
}
bool isSuccess(const OutMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isBinaryDataBuffer(const T&)
{
  return std::is_same_v<T, BinaryDataBuffer>;
}
bool isBinaryDataBuffer(const OutMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isImageBinaryData(const T&)
{
  return std::is_same_v<T, ImageBinaryData>;
}
bool isImageBinaryData(const OutMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isColorImageBinaryData(const T&)
{
  return std::is_same_v<T, ColorImageBinaryData>;
}
bool isColorImageBinaryData(const OutMessage& msg);

template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isError(const T&)
{
  return std::is_same_v<T, Error>;
}
bool isError(const OutMessage& msg);

}  // namespace protocol
}  // namespace irsol
