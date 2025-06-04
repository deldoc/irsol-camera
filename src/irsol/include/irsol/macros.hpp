/**
 * @file irsol/macros.hpp
 * @brief Common portability and diagnostic macros for the irsol library.
 *
 * Provides cross-platform macros to suppress unused variable warnings,
 * indicate unreachable code paths, and enforce template specialization requirements.
 *
 * These utilities help ensure code clarity and maintainability across compilers
 * and standard versions, and are typically used in template metaprogramming,
 * constexpr logic, or when interfacing with platform-specific APIs.
 */

#pragma once

#include "irsol/assert.hpp"
#include "irsol/traits.hpp"

/**
 * @def IRSOL_MAYBE_UNUSED
 * @brief Suppresses compiler warnings about unused variables or parameters.
 *
 * Use this macro to annotate variables or function parameters that are intentionally unused.
 * It adapts to the best syntax supported by the current compiler and language standard.
 * ```cpp
 * IRSOL_MAYBE_UNUSED int unusedVar;
 *
 * void f(IRSOL_MAYBE_UNUSED int unusedParam) {
 *   // do nothing
 * }
 * ```
 */
#if defined(__cplusplus) && __cplusplus >= 201703L
#define IRSOL_MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#define IRSOL_MAYBE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
#define IRSOL_MAYBE_UNUSED
#else
#define IRSOL_MAYBE_UNUSED
#endif

/**
 * @def IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START
 * @brief Begins a diagnostic suppression block for unused structured binding variables.
 *
 * Useful when only part of a structured binding is used inside a loop or block.
 * ```cpp
 * IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START
 * for (auto& [key, value] : map) {
 * IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
 *   doSomethingWith(key);
 * }
 * ```
 *
 * @def IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
 * @brief Ends a diagnostic suppression block started with @ref
 * IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START.
 */
#if defined(__GNUC__) || defined(__clang__)
#ifndef IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START
#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START \
  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
#endif
#ifndef IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END _Pragma("GCC diagnostic pop")
#endif
#else
#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START
#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
#endif

/**
 * @def _IRSOL_UNREACHABLE_IMPL
 * @brief Internal macro for compiler-specific unreachable code markers.
 *
 * Expands to a compiler intrinsic that informs the optimizer the code is unreachable.
 * Do not use directly—use @ref IRSOL_UNREACHABLE instead.
 */
#if defined(__GNUC__) || defined(__clang__)
#define _IRSOL_UNREACHABLE_IMPL() __builtin_unreachable()
#elif defined(_MSC_VER)
#define _IRSOL_UNREACHABLE_IMPL() __assume(false)
#else
#define _IRSOL_UNREACHABLE_IMPL() \
  do {                            \
  } while(0)
#endif

/**
 * @def IRSOL_UNREACHABLE
 * @brief Indicates unreachable code to the compiler.
 *
 * Use this macro in logically unreachable branches (e.g., fully covered switch cases)
 * to suppress warnings and help with optimization.
 * ```cpp
 * int getCode(Type t) {
 *   switch (t) {
 *     case Type::A: return 1;
 *     case Type::B: return 2;
 *   }
 *   IRSOL_UNREACHABLE();
 * }
 * ```
 *
 * @warning If the control flow does reach this macro, undefined behavior may occur.
 */
#define IRSOL_UNREACHABLE() _IRSOL_UNREACHABLE_IMPL()

/**
 * @def IRSOL_STATIC_UNREACHABLE
 * @brief Marks unreachable branches in `constexpr` functions.
 *
 * This macro emits a compile-time error when reached. It is designed for use in
 * `if constexpr` branches that should never be selected at compile-time.
 * ```cpp
 * constexpr int safeDivide(int x) {
 *   if constexpr (x == 0) {
 *     IRSOL_STATIC_UNREACHABLE("Division by zero");
 *   }
 *   return 10 / x;
 * }
 * ```
 *
 * @note If this macro is reached in runtime code (i.e., not `constexpr`), it still falls back
 *       to @ref IRSOL_UNREACHABLE to mark the code as unreachable at runtime.
 *
 * @param messageLiteral A literal C-string message describing the unreachable condition.
 */
#ifndef IRSOL_STATIC_UNREACHABLE
#define IRSOL_STATIC_UNREACHABLE(messageLiteral)                          \
  IRSOL_STATIC_ASSERT(                                                    \
    (irsol::traits::always_false<struct _irsol_dependent_false_>::value), \
    "Unreachable constexpr code triggered: " messageLiteral);             \
  _IRSOL_UNREACHABLE_IMPL()
#endif

/**
 * @def _IRSOL_STATIC_ASSERT_MISSING_TEMPLATE_SPECIALIZATION
 * @brief Internal helper to trigger a static assertion when a required template specialization is
 * missing.
 *
 * This macro should not be used directly. Use @ref IRSOL_MISSING_TEMPLATE_SPECIALIZATION instead.
 *
 * @param T The template type parameter for which specialization is missing.
 * @param messageLiteral A description of the missing specialization.
 */
#ifndef _IRSOL_STATIC_ASSERT_MISSING_TEMPLATE_SPECIALIZATION
#define _IRSOL_STATIC_ASSERT_MISSING_TEMPLATE_SPECIALIZATION(T, messageLiteral) \
  IRSOL_STATIC_ASSERT((irsol::traits::always_false<T>::value), messageLiteral)
#endif

/**
 * @def IRSOL_MISSING_TEMPLATE_SPECIALIZATION
 * @brief Emits a compile-time error when no template specialization is available.
 *
 * This macro is useful in `if constexpr` chains to clearly indicate that a required
 * template specialization has not been provided.
 * ```cpp
 * template <typename T>
 * void process() {
 *   if constexpr (std::is_integral_v<T>) {
 *     // Implementation for integral types
 *   } else {
 *     IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "process");
 *   }
 * }
 * ```
 *
 * @param T The template type parameter.
 * @param funcNameLiteral A string literal representing the function or context.
 */
#ifndef IRSOL_MISSING_TEMPLATE_SPECIALIZATION
#define IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, funcNameLiteral)                    \
  _IRSOL_STATIC_ASSERT_MISSING_TEMPLATE_SPECIALIZATION(                              \
    T,                                                                               \
    "Function '" funcNameLiteral "' lacks a template specialization for this type. " \
    "Please ensure you handle all required template types explicitly.")
#endif
