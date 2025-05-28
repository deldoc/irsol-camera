#pragma once

#ifdef IRSOL_DISABLE_ASSERT
#ifdef PPK_ASSERT_ENABLED
#if PPK_ASSERT_ENABLED
#error "PPK_ASSERT_ENABLED is set to 1, but IRSOL_DISABLE_ASSERT is defined."
#endif
#else
#define PPK_ASSERT_ENABLED 0
#endif
#else
#define PPK_ASSERT_ENABLED 1
#endif

#include "ppk_assert/ppk_assert.hpp"

#define IRSOL_ASSERT_DEBUG \
  PPK_ASSERT_DEBUG  // Prints assertion to console and allows taking interactive action via user
                    // input
#define IRSOL_ASSERT_ERROR PPK_ASSERT_ERROR  // Prints assertion to console and throws an exception
#define IRSOL_ASSERT_FATAL \
  PPK_ASSERT_FATAL  // Prints assertion to console and terminates the program
#define IRSOL_ASSERT_FALSE(message) \
  IRSOL_ASSERT_FATAL(false, message)  // Prints assertion to console and terminates the program

#define IRSOL_STATIC_ASSERT \
  PPK_STATIC_ASSERT  // Prints assertion to console at compile time and terminates the compilation

namespace irsol {
using AssertionException = ppk::assert::AssertionException;  // Exception thrown by PPK_ASSERT_ERROR

typedef ppk::assert::implementation::AssertAction::AssertAction (*AssertHandler)(
  const char* file,
  int         line,
  const char* function,
  const char* expression,
  int         level,
  const char* message);

void setAssertHandler(AssertHandler handler);
void initAssertHandler();

namespace internal {
ppk::assert::implementation::AssertAction::AssertAction assertHandler(
  const char* file,
  int         line,
  const char* function,
  const char* expression,
  int         level,
  const char* message);
}  // namespace internal
}  // namespace irsol