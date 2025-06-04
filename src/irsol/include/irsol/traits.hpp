/**
 * @file irsol/traits.hpp
 * @brief Template metaprogramming traits for type introspection in the irsol library.
 *
 * Provides generic utility templates for controlling function overloads and template specialization
 * based on type traits, including membership in variant types and compile-time guards.
 *
 * @ingroup Metaprogramming
 */

#pragma once

#include "irsol/types.hpp"

#include <type_traits>
#include <variant>

/**
 * @defgroup Metaprogramming Metaprogramming
 * @brief Template traits and utilities for the irsol library.
 *
 * This group contains generic compile-time utilities and type traits used in the irsol codebase,
 * particularly for constraining templates, evaluating type membership in variants, and managing
 * unreachable code paths.
 */

namespace irsol {

/**
 * @namespace irsol::traits
 * @ingroup Metaprogramming
 * @brief Metaprogramming utility namespace.
 *
 * Contains reusable type traits and compile-time logic helpers for constraining template behavior
 * and enabling or disabling overloads based on type relationships.
 */
namespace traits {

/**
 * @ingroup Metaprogramming
 * @brief Always evaluates to `false`, regardless of the type.
 *
 * This trait is typically used in `static_assert` inside `if constexpr` branches that should never
 * be instantiated, serving as a guard for unreachable cases.
 *
 * ```cpp
 * static_assert(irsol::traits::always_false<T>::value, "Unreachable branch encountered");
 * ```
 *
 * @tparam T Any type.
 */
template<typename T>
struct always_false : std::false_type
{};

/**
 * @ingroup Metaprogramming
 * @brief Checks whether a type `T` is one of the types in a `std::variant`.
 *
 * Evaluates to `true` if `T` (after `std::decay`) matches any of the types contained in the
 * specified variant. Used for enabling template functions or static dispatch based on allowed
 * types.
 *
 * ```cpp
 * using VariantType = std::variant<int, float>;
 *
 * static_assert(irsol::traits::is_type_in_variant<int, VariantType>::value, "int is valid");
 * static_assert(!irsol::traits::is_type_in_variant<std::string, VariantType>::value, "string is not
 * valid");
 * ```
 *
 * @tparam T        The type to check.
 * @tparam VariantT A `std::variant` type to check against.
 */
template<typename T, typename VariantT>
struct is_type_in_variant;

template<typename T, typename... VariantClassesT>
struct is_type_in_variant<T, std::variant<VariantClassesT...>>
  : std::disjunction<std::is_same<std::decay_t<T>, VariantClassesT>...>
{};

/**
 * @ingroup Metaprogramming
 * @brief Variable template shorthand for @ref is_type_in_variant.
 *
 * Evaluates to `true` if `T` is in the given variant type. This is a convenience alias
 * for cleaner syntax in `if constexpr` and `enable_if_t` constructs.
 *
 * ```cpp
 * if constexpr (irsol::traits::is_type_in_variant_v<T, MyVariant>) {
 *   // Valid only for types in MyVariant
 * }

 * template <typename T,
 *           std::enable_if_t<irsol::traits::is_type_in_variant_v<T, MyVariant>, int> = 0>
 * void myFunctionTemplate(const T& msg) {
 *   // Only enabled for T types present in MyVariant
 * }
 * ```
 *
 * @tparam T        The type to check.
 * @tparam VariantT A `std::variant` type to check against.
 */
template<typename T, typename VariantT>
constexpr bool is_type_in_variant_v = is_type_in_variant<T, VariantT>::value;

}  // namespace traits
}  // namespace irsol
