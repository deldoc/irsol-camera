#include "irsol/assert.hpp"

#include "irsol/logging.hpp"

#include <ppk_assert/ppk_assert.hpp>

namespace irsol {
void
setAssertHandler(ppk::assert::implementation::AssertHandler handler)
{
  ppk::assert::implementation::setAssertHandler(handler);
}
void
initAssertHandler()
{
  IRSOL_LOG_INFO("Initializing irsol assert handler");
  setAssertHandler(internal::assertHandler);
}

namespace internal {
ppk::assert::implementation::AssertAction::AssertAction
assertHandler(
  const char* file,
  int         line,
  const char* function,
  const char* expression,
  int         level,
  const char* message)
{
  switch(level) {
    case ppk::assert::implementation::AssertLevel::Debug: {
      IRSOL_LOG_WARN(
        "Assertion failed at {0}:{1} -> '{2}' is false: {3}.", file, line, expression, message);
      return ppk::assert::implementation::AssertAction::Ignore;
    }

    case ppk::assert::implementation::AssertLevel::Error: {
      IRSOL_LOG_ERROR(
        "Assertion failed at {0}:{1} -> '{2}' is false: {3}.", file, line, expression, message);
      return ppk::assert::implementation::AssertAction::Throw;
    }
    case ppk::assert::implementation::AssertLevel::Fatal: {
      IRSOL_LOG_FATAL(
        "Assertion failed at {0}:{1} ({2}) -> '{3}' is false: {4}.",
        file,
        line,
        function,
        expression,
        message);
      return ppk::assert::implementation::AssertAction::Abort;
    }
    default: {
#ifdef DEBUG
      IRSOL_LOG_ERROR(
        "Unknown assertion level: {0}, at {1}:{2} -> '{3}' is false: {4}. Aborting program.",
        level,
        file,
        line,
        expression,
        message);
      return ppk::assert::implementation::AssertAction::Throw;
#else
      IRSOL_LOG_FATAL(
        "Unknown assertion level: {0}, at {1}:{2} -> '{3}' is false: {4}. Aborting program.",
        level,
        file,
        line,
        expression,
        message);
      return ppk::assert::implementation::AssertAction::Abort;
#endif
    }
  }
}
}  // namespace internal

}  // namespace irsol