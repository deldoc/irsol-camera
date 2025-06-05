/**
 * @file irsol/protocol/utils.hpp
 * @brief Utility functions for protocol string handling and validation in the irsol library.
 *
 * This header provides helper functions for string processing and validation commonly used
 * in the `irsol::protocol` subsystem. These include:
 * - Validating identifiers against protocol naming rules.
 * - Converting strings to typed values (integers, floating point, strings).
 * - Trimming whitespace from strings.
 *
 * These utilities facilitate safe parsing and validation of protocol messages and parameters.
 */

#pragma once

#include "irsol/logging.hpp"
#include "irsol/macros.hpp"
#include "irsol/types.hpp"

#include <regex>
#include <sstream>
#include <string>
#include <type_traits>

namespace irsol {
namespace protocol {

/**
 * @namespace irsol::protocol::utils
 * @brief Utility functions for string manipulation and validation.
 *
 * Provides low-level helpers for validating protocol identifiers, converting strings
 * to typed values, and trimming whitespace. These functions underpin the protocol
 * parsing logic and ensure consistent input validation and error handling.
 */
namespace utils {

/**
 * @brief Validate that a string is a valid protocol identifier.
 *
 * Checks that the given `identifier` string meets the following rules:
 * - Starts with an alphabetic character (a-z, A-Z) or underscore (_).
 * - Contains only alphanumeric characters, underscores (_), and optionally array indexing
 *   expressions of the form `[number]` (e.g., `param[0]`).
 *
 * If validation fails, logs an error and throws `std::invalid_argument`.
 *
 * @param identifier The string to validate as an identifier.
 * @return The validated identifier string.
 * @throws std::invalid_argument if the string is not a valid identifier.
 *
 * @note This function is intended for validating parameter or field names in protocol messages.
 */
inline std::string
validateIdentifier(const std::string& identifier)
{
  static const std::regex re(R"(^[a-zA-Z_][a-zA-Z0-9_]*(?:\[\d+\])*$)");
  std::smatch             m;
  if(!std::regex_match(identifier, m, re)) {
    IRSOL_LOG_ERROR("Invalid identifier '{}'", identifier);
    throw std::invalid_argument("Invalid identifier");
  }
  return identifier;
}

/**
 * @brief Convert a string to a value of type T.
 *
 * Parses the input string `str` and converts it to the requested type `T`.
 * Supported types:
 * - Integral types (e.g., int, long)
 * - Floating point types (e.g., double, float)
 * - std::string (returns the string as-is)
 *
 * Throws `std::invalid_argument` if the string contains extraneous characters after
 * the parsed value or if conversion fails.
 *
 * @tparam T The desired output type.
 * @param str The string to convert.
 * @return The parsed value of type T.
 * @throws std::invalid_argument on parsing errors.
 *
 * @note Requires explicit template specializations for unsupported types.
 */
template<typename T>
T
fromString(const std::string& str)
{
  std::size_t pos = 0;

  if constexpr(std::is_integral_v<T>) {
    IRSOL_LOG_TRACE("Converting string '{}' to integer", str);
    int res = std::stoi(str, &pos);
    if(pos != str.length())
      throw std::invalid_argument("Extra characters after integer");
    return res;
  } else if constexpr(std::is_floating_point_v<T>) {
    IRSOL_LOG_TRACE("Converting string '{}' to double", str);
    double res = std::stod(str, &pos);
    if(pos != str.length())
      throw std::invalid_argument("Extra characters after double");
    return res;
  } else if constexpr(std::is_same_v<T, std::string>) {
    return str;
  } else {
    IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "utils::fromString()");
  }
}

/**
 * @brief Remove leading and trailing whitespace from a string.
 *
 * Trims spaces, tabs, carriage returns, and newline characters from both
 * ends of the input string `s`.
 *
 * @param s The input string to trim.
 * @return A copy of `s` with leading and trailing whitespace removed.
 *
 * @note Does not modify the original string.
 */
inline std::string
trim(const std::string& s)
{
  size_t start = s.find_first_not_of(" \t\r\n");
  size_t end   = s.find_last_not_of(" \t\r\n");
  return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

}  // namespace utils
}  // namespace protocol
}  // namespace irsol
