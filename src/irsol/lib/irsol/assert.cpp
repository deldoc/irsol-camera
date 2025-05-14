#include "irsol/assert.hpp"
#include "irsol/logging.hpp"
#include "ppk_assert/ppk_assert.hpp"

namespace irsol {
void set_assert_handler(ppk::assert::implementation::AssertHandler handler) {
  ppk::assert::implementation::setAssertHandler(handler);
}
void init_assert_handler() {
  IRSOL_LOG_INFO("Initializing irsol assert handler");
  set_assert_handler(internal::assert_handler);
}

namespace internal {
ppk::assert::implementation::AssertAction::AssertAction
assert_handler(const char *file, int line, const char *function, const char *expression, int level,
               const char *message) {
  switch (level) {
  case ppk::assert::implementation::AssertLevel::Debug:
    IRSOL_LOG_WARN("Assertion failed at {0}:{1} -> '{2}' is false: {3}.", file, line, expression,
                   message);
    return ppk::assert::implementation::AssertAction::Ignore;

  case ppk::assert::implementation::AssertLevel::Error:
    IRSOL_LOG_ERROR("Assertion failed at {0}:{1} -> '{2}' is false: {3}.", file, line, expression,
                    message);
    return ppk::assert::implementation::AssertAction::Throw;
  case ppk::assert::implementation::AssertLevel::Fatal:
    IRSOL_LOG_FATAL("Assertion failed at {0}:{1} ({2}) -> '{3}' is false: {4}.", file, line,
                    function, expression, message);
    return ppk::assert::implementation::AssertAction::Abort;
  default:
    IRSOL_LOG_ERROR(
        "Unknown assertion level: {0}, at {1}:{2} -> '{3}' is false: {4}. Aborting program.", level,
        file, line, expression, message);
    return ppk::assert::implementation::AssertAction::Throw;
  }
}
} // namespace internal

} // namespace irsol