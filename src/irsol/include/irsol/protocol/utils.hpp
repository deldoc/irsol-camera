#pragma once
#include "irsol/logging.hpp"
#include "irsol/protocol/types.hpp"

#include <sstream>
#include <string>
#include <type_traits>

namespace irsol {
namespace protocol {
namespace utils {

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
    static_assert(sizeof(T) == 0, "fromString: unsupported type");
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
