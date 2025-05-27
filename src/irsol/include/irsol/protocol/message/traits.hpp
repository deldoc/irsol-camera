#pragma once

#include "irsol/types.hpp"

#include <variant>

namespace irsol {
namespace protocol {
namespace internal {

namespace traits {

// Create trait helpers to limit the compilation possibility of some template functions
// to only types that are part of the value_t variant.

template<typename T, typename Variant>
struct _IsTypeInVariant;

template<typename T, typename... Ts>
struct _IsTypeInVariant<T, std::variant<Ts...>> : std::disjunction<std::is_same<T, Ts>...>
{};

// To be used as:
// template <typename T, std::enable_if_t<traits::IsInValueTVariant<T>::value, int> = 0>
// void myFunctionTemplate(const T& msg) { ... }
// in this way, the 'myFunctionTemplate' will only accept types that are part of the value_t
// variant. and if used with a type that is not part of the value_t variant, the compiler will
// issue a compilation error.
template<typename T>
using IsInValueTVariant = _IsTypeInVariant<T, ::irsol::types::protocol_value_t>;

}

}
}
}
