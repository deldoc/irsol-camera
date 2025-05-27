#pragma once

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
///  IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
/// <code using only one of key or value>
/// }
#if defined(__GNUC__) || defined(__clang__)
#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START \
  _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")

#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END _Pragma("GCC diagnostic pop")
#else
#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_START
#define IRSOL_SUPPRESS_UNUSED_STRUCTURED_BINDING_END
#endif
