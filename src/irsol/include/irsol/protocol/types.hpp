#pragma once
#include <string>
#include <variant>
namespace irsol {
namespace protocol {
namespace internal {

using value_t = std::variant<int, double, std::string>;
using byte_t  = uint8_t;
}
}
}
