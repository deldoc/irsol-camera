/**
 * @file irsol/utils.hpp
 * @brief General utility functions used throughout the `irsol` library.
 *
 * This header provides helper functions for type conversions, string manipulation,
 * timestamp formatting, and camera device management via the NeoAPI.
 */
#pragma once

#include "irsol/queue.hpp"
#include "irsol/types.hpp"

#include <neoapi/neoapi.hpp>
#include <string>
#include <utility>
#include <vector>

namespace irsol {
/**
 * @namespace irsol::utils
 * @brief Contains general utility functions used throughout the `irsol` library.
 *
 * This namespace provides helper functions for type conversions,
 * string manipulations, timestamp formatting, and camera device management.
 */
namespace utils {

/**
 * @brief Converts a protocol value to an integer.
 *
 * Extracts and returns the underlying `int` from the variant type
 * @ref irsol::types::protocol_value_t.
 *
 * @param x The protocol value to convert.
 * @return The integer representation of the input value.
 * @throws std::bad_variant_access if the variant does not hold an int.
 */
inline int
toInt(const irsol::types::protocol_value_t& x)
{
  return std::get<int>(x);
}

/**
 * @brief Converts a protocol value to a double.
 *
 * Extracts and returns the underlying `double` from the variant type
 * @ref irsol::types::protocol_value_t.
 *
 * @param x The protocol value to convert.
 * @return The double representation of the input value.
 * @throws std::bad_variant_access if the variant does not hold a double.
 */
inline double
toDouble(const irsol::types::protocol_value_t& x)
{
  return std::get<double>(x);
}

/**
 * @brief Converts a protocol value to a string.
 *
 * Extracts and returns the underlying `std::string` from the variant type
 * @ref irsol::types::protocol_value_t.
 *
 * @param x The protocol value to convert.
 * @return The string representation of the input value.
 * @throws std::bad_variant_access if the variant does not hold a string.
 */
inline std::string
toString(const irsol::types::protocol_value_t& x)
{
  return std::get<std::string>(x);
}

/**
 * @brief Generates a new UUID string.
 *
 * Generates a version 4 (random) UUID and returns it as a string.
 *
 * @return A newly generated UUID string, e.g., "123e4567-e89b-12d3-a456-426614174000".
 */
std::string uuid();

/**
 * @brief Splits a string into tokens based on a delimiter.
 *
 * Parses the input string `s` and splits it into substrings each time the
 * character `delimiter` is encountered.
 *
 * @param s The string to split.
 * @param delimiter The character to use as the delimiter.
 * @return A vector of token substrings.
 */
std::vector<std::string> split(const std::string& s, char delimiter);

/**
 * @brief Removes leading and trailing characters from a string.
 *
 * Removes any characters contained in the `delimiters` string from both
 * the start and the end of the input string `s`. Useful for trimming whitespace
 * or custom characters.
 *
 * @param s The input string to process.
 * @param delimiters A string containing all characters to strip (default is whitespace characters).
 * @return A new string with specified characters removed from both ends.
 */
std::string strip(const std::string& s, const std::string& delimiters = " \t\r\n");

/**
 * @brief Removes all occurrences of a specific substring from the start and end of a string.
 *
 * If the string `s` begins or ends with the substring `strippedString`,
 * those occurrences are removed. This is repeated until `s` no longer starts or ends with that
 * substring.
 *
 * @param s The original string.
 * @param strippedString The substring to remove from both ends.
 * @return A new string with the specified substring removed from start and end.
 */
std::string stripString(const std::string& s, const std::string& strippedString);

/**
 * @brief Converts a steady_clock time point to a human-readable string.
 *
 * Converts a @ref irsol::types::timepoint_t into a formatted string representation.
 *
 * @param tp The time point to convert.
 * @return A human-readable string representation of the time point.
 */
std::string timestampToString(irsol::types::timepoint_t tp);

/**
 * @brief Converts a duration to a human-readable string.
 *
 * Converts a @ref irsol::types::duration_t into a formatted string representing the duration.
 *
 * @param dr The duration to convert.
 * @return A human-readable string representation of the duration.
 */
std::string durationToString(irsol::types::duration_t dr);

/**
 * @brief Loads the default camera device.
 *
 * Queries the NeoAPI for connected cameras and attempts to open the camera
 * with the default serial number defined in @ref
 * irsol::utils::internal::defaultCameraSerialNumber().
 *
 * @throws std::runtime_error If no matching camera is found or initialization fails.
 * @return A handle to the opened NeoAPI camera device.
 */
NeoAPI::Cam loadDefaultCamera();

/**
 * @brief Discovers all cameras connected to the system.
 *
 * Uses NeoAPI to scan and retrieve information about all connected cameras.
 * The returned reference remains valid until the next call to `discoverCameras()`.
 *
 * @return A reference to a list of all discovered cameras.
 */
NeoAPI::CamInfoList& discoverCameras();

/**
 * @namespace irsol::utils::internal
 * @brief Contains internal helper constants and functions.
 *
 * This namespace holds implementation details and constants
 * used internally by the @ref irsol::utils namespace. Users
 * typically do not need to use this namespace directly.
 */
namespace internal {

/**
 * @brief Default serial number for selecting the system camera.
 *
 * A compile-time constant string identifying the preferred or fallback
 * camera serial number used by @ref irsol::utils::loadDefaultCamera.
 *
 * @return The default camera serial number as a null-terminated C-string.
 */
constexpr const char* defaultCameraSerialNumber();

}  // namespace internal
}  // namespace utils
}  // namespace irsol
