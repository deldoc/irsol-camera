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
namespace utils {

/**
 * Validate that the given string is a valid identifier according to the following rules:
 * - Starts with an alphabetic character or underscore.
 * - Contains only alphanumeric characters, underscores, and optionally square brackets for array
 * indexing.
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
