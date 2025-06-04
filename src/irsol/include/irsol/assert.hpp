// clang-format off
/**
 * @file irsol/assert.hpp
 * @ingroup Assertion
 * @brief Assertion macros and utilities based on the
 * [PPK_ASSERT](HTTPS://GITHUB.COM/GPAKOSZ/PPK_ASSERT) library.
 *
 * This header provides assertion macros with configurable behavior,
 * exception types, and customizable assertion handlers for the irsol
 * library. It wraps and controls usage of the underlying
 * [PPK_ASSERT](HTTPS://GITHUB.COM/GPAKOSZ/PPK_ASSERT) assertion framework, allowing fine-grained
 * control over debug, error, and fatal assertions.
 *
 * The header supports disabling assertions via `IRSOL_DISABLE_ASSERT`,
 * with compile-time checks to prevent conflicting macros.
 *
 * For more details on the usage of assertion mechanism, please see @ref logging_and_asserting.
 * Assertions help catch programming errors and invalid states. This system supports multiple assertion levels with distinct semantics:
 * | Level                | When to Use                                | Behavior in `Debug` Mode                              | Behavior in `Release` Mode                            |
 * | -------------------- | ------------------------------------------ | ----------------------------------------------------- | ----------------------------------------------------- |
 * | `IRSOL_ASSERT_DEBUG` | Check internal invariants, sanity checks   | Logs warning, continues execution (ignored)           | Logs warning, continues execution (ignored)           |
 * | `IRSOL_ASSERT_ERROR` | Recoverable errors, invalid input detected | Logs error and throws an exception that can be caught | Logs error and throws an exception that can be caught |
 * | `IRSOL_ASSERT_FATAL` | Critical failure, unrecoverable state      | Logs error and aborts program immediately             | Logs fatal and aborts program immediately             |
 */
// clang-format on

#pragma once

/**
 * @defgroup Assertion Assertion
 * @brief Assertion macros, types, and functions for the irsol library.
 *
 * Provides compile-time and runtime assertion mechanisms using the PPK_ASSERT library.
 */

/**
 * @def IRSOL_DISABLE_ASSERT
 * @ingroup Assertion
 * @brief When defined at compile time, disables all irsol assertions.
 *
 * This disables all assertion macros by disabling the underlying
 * [PPK_ASSERT](HTTPS://GITHUB.COM/GPAKOSZ/PPK_ASSERT). If `PPK_ASSERT_ENABLED` is set to 1
 * simultaneously, a compilation error is generated.
 */

/**
 * @def IRSOL_ASSERT_DEBUG
 * @ingroup Assertion
 * @brief Debug-level assertion macro.
 *
 * Prints assertion failure details to the console and allows interactive
 * user action (e.g., ignore, retry, abort). Typically used for non-critical
 * conditions during development.
 *
 * Usage:
 * @code
 * IRSOL_ASSERT_DEBUG(x > 0, "x must be positive");
 * @endcode
 */

/**
 * @def IRSOL_ASSERT_ERROR
 * @ingroup Assertion
 * @brief Error-level assertion macro.
 *
 * Prints assertion failure details to the console and throws an
 * exception of type `irsol::AssertionException`. Use for recoverable
 * error conditions that should be handled via exception handling.
 *
 * Usage:
 * @code
 * IRSOL_ASSERT_ERROR(ptr != nullptr, "Pointer must not be null");
 * @endcode
 */

/**
 * @def IRSOL_ASSERT_FATAL
 * @ingroup Assertion
 * @brief Fatal-level assertion macro.
 *
 * Prints assertion failure details to the console and immediately
 * terminates the program. Use for unrecoverable conditions that
 * require aborting execution.
 *
 * Usage:
 * @code
 * IRSOL_ASSERT_FATAL(connected, "Failed to connect to server");
 * @endcode
 */

/**
 * @def IRSOL_ASSERT_FALSE
 * @ingroup Assertion
 * @brief Assertion macro that always fails fatally.
 *
 * Prints the provided message and terminates the program immediately.
 * Useful for marking code paths that should never be reached.
 *
 * Usage:
 * @code
 * IRSOL_ASSERT_FALSE("Unreachable code executed");
 * @endcode
 */

/**
 * @def IRSOL_STATIC_ASSERT
 * @ingroup Assertion
 * @brief Compile-time static assertion macro.
 *
 * Evaluates the condition at compile time and terminates compilation
 * if the condition is false, printing the provided message.
 *
 * Usage:
 * @code
 * IRSOL_STATIC_ASSERT(sizeof(int) == 4, "int size must be 4 bytes");
 * @endcode
 */

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

#include <ppk_assert/ppk_assert.hpp>

#define IRSOL_ASSERT_DEBUG PPK_ASSERT_DEBUG
#define IRSOL_ASSERT_ERROR PPK_ASSERT_ERROR
#define IRSOL_ASSERT_FATAL PPK_ASSERT_FATAL
#define IRSOL_ASSERT_FALSE(message) IRSOL_ASSERT_FATAL(false, message)

#define IRSOL_STATIC_ASSERT PPK_STATIC_ASSERT

namespace irsol {

/**
 * @typedef AssertionException
 * @ingroup Assertion
 * @brief Exception type thrown by @ref IRSOL_ASSERT_ERROR macro on assertion failure.
 *
 * This is an alias for `ppk::assert::AssertionException`.
 */
using AssertionException = ppk::assert::AssertionException;

/**
 * @typedef AssertHandler
 * @ingroup Assertion
 * @brief Type for a custom assertion handler function.
 *
 * The handler is called on assertion failures with details such as file,
 * line number, function, expression, level, and message.
 *
 * It must return an `ppk::assert::implementation::AssertAction::AssertAction` value indicating how
 * to proceed.
 *
 * @param file The source file where the assertion failed.
 * @param line The line number in the source file.
 * @param function The function name containing the assertion.
 * @param expression The asserted expression as a string.
 * @param level The assertion severity level.
 * @param message An optional descriptive message.
 * @return The action to take in response to the assertion failure.
 */
typedef ppk::assert::implementation::AssertAction::AssertAction (*AssertHandler)(
  const char* file,
  int         line,
  const char* function,
  const char* expression,
  int         level,
  const char* message);

/**
 * @ingroup Assertion
 * @brief Sets a custom assertion handler function.
 *
 * This function installs a user-defined callback that is invoked whenever
 * an assertion fails. The handler controls the response behavior.
 *
 * @param handler The user-defined assertion handler to set.
 */
void setAssertHandler(AssertHandler handler);

/**
 * @ingroup Assertion
 * @brief Initializes the assertion handler system.
 *
 * Call this to configure any required internal state before assertions
 * are used. Typically called at program startup.
 *
 * Sets a custom assertion handler that captures assertions, and based on the build type reacts to
 * them.
 *
 * @see setAssertHandler
 * @see irsol::internal::AssertHandler
 */
void initAssertHandler();

namespace internal {

/**
 * @ingroup Assertion
 * @brief Internal default assertion handler implementation.
 *
 * Handles assertion failures by processing and dispatching
 * them according to the current assertion policy.
 *
 * This function should not be called directly; it is called by
 * the [PPK_ASSERT](HTTPS://GITHUB.COM/GPAKOSZ/PPK_ASSERT) framework internally.
 *
 * @param file The source file of the assertion failure.
 * @param line The line number of the failure.
 * @param function The function containing the assertion.
 * @param expression The asserted expression.
 * @param level The assertion severity level.
 * @param message Optional descriptive message.
 * @return The action to take (e.g., ignore, retry, abort).
 */
ppk::assert::implementation::AssertAction::AssertAction assertHandler(
  const char* file,
  int         line,
  const char* function,
  const char* expression,
  int         level,
  const char* message);

}  // namespace internal
}  // namespace irsol
