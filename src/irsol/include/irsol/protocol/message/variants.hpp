/**
 * @file irsol/protocol/message/variants.hpp
 * @brief Definitions and utilities for incoming and outgoing protocol messages.
 *
 * This header provides the core types, enums, and helper functions related to
 * the representation and handling of protocol messages exchanged in the irsol system.
 *
 * It defines:
 * - Enumerations for distinguishing kinds of incoming (`InMessageKind`) and outgoing
 * (`OutMessageKind`) messages.
 * - Variant types (`InMessage`, `OutMessage`) that represent all supported message types.
 * - String conversion functions for message kinds and messages.
 * - Compile-time and runtime utilities to identify and check message types.
 *
 * The incoming messages cover Assignments, Inquiries, and Commands.
 * The outgoing messages cover Success acknowledgments, Binary data buffers, Black & White images,
 * Color images, and Error reports.
 *
 * @ingroup Protocol
 */

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
 *
 * Incoming messages are categorized into three kinds:
 * - ASSIGNMENT: Messages that assign values or configurations.
 * - INQUIRY: Messages that request information.
 * - COMMAND: Messages that trigger actions.
 * @ingroup Protocol
 */
enum class InMessageKind
{
  ASSIGNMENT,
  INQUIRY,
  COMMAND
};

/**
 * @brief Converts an InMessageKind enum to a human-readable string.
 * @param kind The InMessageKind value to convert.
 * @return A string representation of the given InMessageKind.
 * @note This function is constexpr and can be used in compile-time contexts.
 * @ingroup Protocol
 */
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
  IRSOL_UNREACHABLE();
}

// Forward declarations of incoming message types
struct Assignment;
struct Inquiry;
struct Command;

/**
 * @brief Variant type representing any incoming message.
 *
 * This variant holds one of the possible incoming message types:
 * - Assignment
 * - Inquiry
 * - Command
 * @ingroup Protocol
 */
using InMessage = std::variant<Assignment, Inquiry, Command>;

/**
 * @brief Represents the type of an outgoing message.
 *
 * Outgoing messages are categorized into:
 * - SUCCESS: Acknowledgement of successful operation.
 * - BINARY_BUFFER: Raw binary data payload.
 * - BW_IMAGE: Black & White image data.
 * - COLOR_IMAGE: Color image data.
 * - ERROR: Error message.
 * @ingroup Protocol
 */
enum class OutMessageKind
{
  SUCCESS,
  BINARY_BUFFER,
  BW_IMAGE,
  COLOR_IMAGE,
  ERROR
};

/**
 * @brief Converts an OutMessageKind enum to a human-readable string.
 * @param kind The OutMessageKind value to convert.
 * @return A string representation of the given OutMessageKind.
 * @note This function is constexpr and can be used in compile-time contexts.
 * @ingroup Protocol
 */
constexpr const char*
OutMessageKindToString(OutMessageKind kind)
{
  switch(kind) {
    case OutMessageKind::SUCCESS:
      return "SUCCESS";
    case OutMessageKind::BINARY_BUFFER:
      return "BINARY_BUFFER";
    case OutMessageKind::BW_IMAGE:
      return "BW_IMAGE";
    case OutMessageKind::COLOR_IMAGE:
      return "COLOR_IMAGE";
    case OutMessageKind::ERROR:
      return "ERROR";
  }
  IRSOL_UNREACHABLE();
}

// Forward declarations of outgoing message types
struct Success;
struct Error;

/**
 * @brief Variant type representing any outgoing message.
 *
 * This variant holds one of the possible outgoing message types:
 * - Success
 * - BinaryDataBuffer
 * - ImageBinaryData (black & white)
 * - ColorImageBinaryData
 * - Error
 * @ingroup Protocol
 */
using OutMessage =
  std::variant<Success, BinaryDataBuffer, ImageBinaryData, ColorImageBinaryData, Error>;

/**
 * @brief Converts an incoming message variant to a human-readable string.
 * @param msg The incoming message variant.
 * @return A string representation of the contained message.
 * @ingroup Protocol
 */
std::string toString(const InMessage& msg);

/**
 * @brief Converts an outgoing message variant to a human-readable string.
 * @param msg The outgoing message variant.
 * @return A string representation of the contained message.
 * @ingroup Protocol
 */
std::string toString(const OutMessage& msg);

/**
 * @brief Returns the InMessageKind enum value corresponding to a given incoming message type.
 * @tparam T The concrete incoming message type.
 * @param msg The incoming message instance (unused).
 * @return The corresponding InMessageKind value.
 * @note This function is constexpr and uses SFINAE to constrain T to be one of the InMessage
 * variant types.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
constexpr InMessageKind
getInMessageKind(IRSOL_MAYBE_UNUSED const T& msg)
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
 * @brief Returns the InMessageKind enum value for a given incoming message variant.
 * @param msg The incoming message variant.
 * @return The corresponding InMessageKind value.
 * @ingroup Protocol
 */
InMessageKind getInMessageKind(const InMessage& msg);

/**
 * @brief Checks if a given incoming message type is an Assignment.
 * @tparam T The concrete incoming message type.
 * @param msg The incoming message instance.
 * @return True if T is Assignment, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
constexpr bool
isAssignment(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, Assignment>;
}
bool isAssignment(const InMessage& msg);

/**
 * @brief Checks if a given incoming message type is an Inquiry.
 * @tparam T The concrete incoming message type.
 * @param msg The incoming message instance.
 * @return True if T is Inquiry, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
constexpr bool
isInquiry(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, Inquiry>;
}
bool isInquiry(const InMessage& msg);

/**
 * @brief Checks if a given incoming message type is a Command.
 * @tparam T The concrete incoming message type.
 * @param msg The incoming message instance.
 * @return True if T is Command, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, InMessage>, int> = 0>
constexpr bool
isCommand(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, Command>;
}
bool isCommand(const InMessage& msg);

/**
 * @brief Returns the OutMessageKind enum value corresponding to a given outgoing message type.
 * @tparam T The concrete outgoing message type.
 * @param msg The outgoing message instance (unused).
 * @return The corresponding OutMessageKind value.
 * @note This function is constexpr and uses SFINAE to constrain T to be one of the OutMessage
 * variant types.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr OutMessageKind
getOutMessageKind(IRSOL_MAYBE_UNUSED const T& msg)
{
  if constexpr(std::is_same_v<T, Success>)
    return OutMessageKind::SUCCESS;
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
 * @brief Returns the OutMessageKind enum value for a given outgoing message variant.
 * @param msg The outgoing message variant.
 * @return The corresponding OutMessageKind value.
 * @ingroup Protocol
 */
OutMessageKind getOutMessageKind(const OutMessage& msg);

/**
 * @brief Checks if a given outgoing message type is Success.
 * @tparam T The concrete outgoing message type.
 * @param msg The outgoing message instance.
 * @return True if T is Success, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isSuccess(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, Success>;
}
bool isSuccess(const OutMessage& msg);

/**
 * @brief Checks if a given outgoing message type is BinaryDataBuffer.
 * @tparam T The concrete outgoing message type.
 * @param msg The outgoing message instance.
 * @return True if T is BinaryDataBuffer, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isBinaryDataBuffer(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, BinaryDataBuffer>;
}
bool isBinaryDataBuffer(const OutMessage& msg);

/**
 * @brief Checks if a given outgoing message type is ImageBinaryData (black & white).
 * @tparam T The concrete outgoing message type.
 * @param msg The outgoing message instance.
 * @return True if T is ImageBinaryData, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isImageBinaryData(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, ImageBinaryData>;
}
bool isImageBinaryData(const OutMessage& msg);

/**
 * @brief Checks if a given outgoing message type is ColorImageBinaryData.
 * @tparam T The concrete outgoing message type.
 * @param msg The outgoing message instance.
 * @return True if T is ColorImageBinaryData, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isColorImageBinaryData(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, ColorImageBinaryData>;
}
bool isColorImageBinaryData(const OutMessage& msg);

/**
 * @brief Checks if a given outgoing message type is Error.
 * @tparam T The concrete outgoing message type.
 * @param msg The outgoing message instance.
 * @return True if T is Error, false otherwise.
 * @ingroup Protocol
 */
template<typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, OutMessage>, int> = 0>
constexpr bool
isError(IRSOL_MAYBE_UNUSED T& msg)
{
  return std::is_same_v<T, Error>;
}
bool isError(const OutMessage& msg);

}  // namespace protocol
}  // namespace irsol
