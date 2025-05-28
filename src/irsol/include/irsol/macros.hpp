#pragma once

#include "irsol/assert.hpp"
#include "irsol/traits.hpp"

/// This macro is used to mark variables or function parameters as unused. This mutes the compiler's
/// warning about unused variables.
/// Example usage:
/// ```cpp
/// IRSOL_MAYBE_UNUSED int unusedVariable;
/// ```
/// or for function parameters:
/// ```cpp
/// void MyFunction(IRSOL_MAYBE_UNUSED int unusedParameter) {
/// ...
/// }
/// ```
#if defined(__cplusplus) && __cplusplus >= 201703L
#define IRSOL_MAYBE_UNUSED [[maybe_unused]]
#elif defined(__GNUC__) || defined(__clang__)
#define IRSOL_MAYBE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
#define IRSOL_MAYBE_UNUSED
#else
#define IRSOL_MAYBE_UNUSED
#endif

/// Wrap unused structured binding members
/// Example usage:
/// ```cpp
/// IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START
/// for(auto& [key, value] : myMap) {
///    IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
///    // code using only one of key or value
/// }
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

/// Helper for IRSOL_UNREACHABLE.
#if defined(__GNUC__) || defined(__clang__)
#define _IRSOL_UNREACHABLE_IMPL() __builtin_unreachable()
#elif defined(_MSC_VER)
#define _IRSOL_UNREACHABLE_IMPL() __assume(false)
#else
#define _IRSOL_UNREACHABLE_IMPL() \
  do {                            \
  } while(0)
#endif

/// Indicates unreachable code.
/// Example:
/// ```cpp
/// int MyFunction() {
///  if (<always true>) {
///    ...
///    return 1;
///  }
///  IRSOL_UNREACHABLE();
/// ```
/// This is useful for functions containing switch statements covering all possible cases,
/// and for which you don't want to have a default dummy case or a compiler warning.
/// Note: this macro is a simple compiler hint, and if the execution flow reaches this point,
/// undefined behaviour may occur.
#define IRSOL_UNREACHABLE() _IRSOL_UNREACHABLE_IMPL()

/// Indicates unreachable code in constexpr functions.
/// Example:
/// ```cpp
/// constexpr int foo(int value) {
///   if constexpr (value == 0) {
///     IRSOL_STATIC_UNREACHABLE("Unexpected code path");
///   }
///   return value;
/// };
/// ```
/// Note: this macro should only be used in constexpr functions.
///       If code is complied and the compilation logic hits this macro, it will trigger a
///       compile-time error. So, following the example, if `foo` is called with a literal value
///       `foo(0)`, a compile-time error will be triggered.
#ifndef IRSOL_STATIC_UNREACHABLE
#define IRSOL_STATIC_UNREACHABLE(messageLiteral)                          \
  IRSOL_STATIC_ASSERT(                                                    \
    (irsol::traits::always_false<struct _irsol_dependent_false_>::value), \
    "Unreachable constexpr code triggered: " messageLiteral);             \
  _IRSOL_UNREACHABLE_IMPL()
#endif

/// Used in template code when no valid specialization exists.
/// To be used as a final `else` branch in `if constexpr` chains.
#ifndef _IRSOL_STATIC_ASSERT_MISSING_TEMPLATE_SPECIALIZATION
#define _IRSOL_STATIC_ASSERT_MISSING_TEMPLATE_SPECIALIZATION(T, messageLiteral) \
  IRSOL_STATIC_ASSERT((irsol::traits::always_false<T>::value), messageLiteral)
#endif

/// User-facing macro to emit a compile-time error when a template specialization is missing.
/// Can be safely used as a single statement inside functions.
/// Example:
/// ```cpp
/// template<typename T>
/// void MyFunction() {
///   if constexpr (std::is_integral<T>::value) {
///      // Implementation for integral types
///   } else {
///      IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, "MyFunction");
///   }
/// }
/// ```
#ifndef IRSOL_MISSING_TEMPLATE_SPECIALIZATION
#define IRSOL_MISSING_TEMPLATE_SPECIALIZATION(T, funcNameLiteral)                    \
  _IRSOL_STATIC_ASSERT_MISSING_TEMPLATE_SPECIALIZATION(                              \
    T,                                                                               \
    "Function '" funcNameLiteral "' lacks a template specialization for this type. " \
    "Please ensure you handle all required template types explicitly.")
#endif
