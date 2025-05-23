#pragma once

#include "neoapi/neoapi.hpp"

#include <chrono>
#include <string>
#include <utility>
#include <vector>

namespace irsol {
namespace utils {

/**
 * @brief Generate a new UUID string.
 *
 * @return A string representing the UUID (e.g., "123e4567-e89b-12d3-a456-426614174000").
 */
std::string uuid();

/**
 * @brief Split a string into tokens based on a delimiter.
 *
 * Parses the input string `s`, splitting at each occurrence of
 * the character `delimiter`, and returns a vector of token substrings.
 *
 * @param s The input string to split.
 * @param delimiter The character at which to split.
 * @return A vector of substrings.
 */
std::vector<std::string> split(const std::string& s, char delimiter);

/**
 * @brief Strip leading and trailing characters from a string.
 *
 * Removes any characters in `delimiters` from the start and end
 * of the input string `s`. Useful for trimming whitespace or
 * specific punctuation.
 *
 * @param s The input string to strip.
 * @param delimiters A string containing all characters to remove.
 * @return A new string with the specified characters removed from both ends.
 */
std::string strip(const std::string& s, const std::string& delimiters = " \t\r\n ");

/**
 * @brief Remove all occurrences of a substring from a string start and end.
 *
 *
 * @param s The original string.
 * @param strippedString The substring to remove.
 * @return A new string with all occurrences of `strippedString` removed from its start and end.
 */
std::string stripString(const std::string& s, const std::string& strippedString);

/**
 * @brief Convert a steady_clock time point to a human-readable string.
 *
 * @param tp The time point to convert.
 * @return A string representation of the time point.
 */
std::string timestamp_to_str(std::chrono::steady_clock::time_point tp);

/**
 * @brief Load the default camera device.
 *
 * Queries the NeoAPI for available cameras and returns the first
 * matching the default serial number defined in internal::defaultCameraSerialNumber().
 * Throws an exception if no camera is found or if initialization fails.
 *
 * @return A NeoAPI::Cam handle to the opened camera device.
 */
NeoAPI::Cam loadDefaultCamera();

/**
 * @brief Log information about a camera to the application logger.
 *
 * Extracts relevant fields from the NeoAPI::CamInfo struct and logs
 * them at INFO level, including serial number, model name, resolution,
 * and supported features.
 *
 * @param info The camera info structure to log.
 */
void logCameraInfo(const NeoAPI::CamInfo& info);

/**
 * @brief Discover all cameras connected to the system.
 *
 * Queries NeoAPI for a list of connected camera devices and returns
 * a reference to the internal camera info list. The returned reference
 * remains valid until the next call to discoverCameras().
 *
 * @return A reference to a NeoAPI::CamInfoList containing all discovered cameras.
 */
NeoAPI::CamInfoList& discoverCameras();

namespace internal {

/**
 * @brief Default serial number for selecting the system camera.
 *
 * Provides a compile-time constant string identifying the
 * preferred or fallback camera serial number. Used by loadDefaultCamera().
 *
 * @return A C-string of the default camera serial (e.g., "ABC1234").
 */
constexpr const char* defaultCameraSerialNumber();

}  // namespace internal
}  // namespace utils
}  // namespace irsol
