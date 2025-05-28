#pragma once

#include "irsol/types.hpp"

#include <variant>
namespace irsol {
namespace traits {

// Create trait helpers to limit the compilation possibility of some template functions
// to only types that are part of the value_t variant.
template<typename T, typename VariantT>
struct is_type_in_variant;

template<typename T, typename... VariantClassesT>
struct is_type_in_variant<T, std::variant<VariantClassesT...>>
  : std::disjunction<std::is_same<std::decay_t<T>, VariantClassesT>...>
{};

// To be used as:
// template <typename T, std::enable_if_t<irsol::traits::is_type_in_variant_v<T, MyVariant>, int> =
// 0> void myFunctionTemplate(const T& msg) { ... } in this way, the 'myFunctionTemplate' will only
// accept types that are part of the MyVariant variant.
template<typename T, typename VariantT>
constexpr bool is_type_in_variant_v = is_type_in_variant<T, VariantT>::value;

}  // namespace traits
}  // namespace irsol