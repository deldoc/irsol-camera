#include "irsol/assert.hpp"
#include "irsol/logging.hpp"

#include <math.h>

/// @brief Returns the program name, typically used for logging.
/// If `PROGRAM_NAME` is not defined at compile time, returns `"logging-and-asserting"`.
const std::string
getProgramName()
{
#ifndef PROGRAM_NAME
#define PROGRAM_NAME "logging-and-asserting"
#endif
  return PROGRAM_NAME;
}

void
demoLogging()
{
  IRSOL_LOG_TRACE("This is a trace message.");
  IRSOL_LOG_DEBUG("This is a debug message.");
  IRSOL_LOG_INFO("This is an info message.");
  IRSOL_LOG_WARN("This is a warning message.");
  IRSOL_LOG_ERROR("This is an error message.");
  IRSOL_LOG_FATAL("This is a fatal message.");

  IRSOL_NAMED_LOG_INFO("example", "This is an info message from a named logger '{}'.", "example");
}

void
demoLoggingWithArguments()
{
  // Logging arguments has never been easier
  IRSOL_LOG_INFO(
    "This is a log message with {} arguments: {} and {}", 2, "first-argument is a string", 0.5);
  IRSOL_LOG_INFO("We can determine the precision for floating point numbers;");
  IRSOL_LOG_INFO("PI: {}", M_PI);
  IRSOL_LOG_INFO("PI: {:.2f}", M_PI);
}

void
demoPassingAssertions()
{
  IRSOL_LOG_INFO("Testing assertions that pass...");

  int value = 42;

  IRSOL_LOG_TRACE("Running debug assertion");
  IRSOL_ASSERT_DEBUG(value == 42, "Debug assertion passed: value is 42");
  IRSOL_LOG_TRACE("Running error assertion");
  IRSOL_ASSERT_ERROR(value == 42, "Error assertion passed: value is 42");
  IRSOL_LOG_TRACE("Running fatal assertion");
  IRSOL_ASSERT_FATAL(value == 42, "Fatal assertion passed: value is 42");

  IRSOL_LOG_INFO("All passing assertions succeeded.");
}

void
demoFailingAssertions()
{
  IRSOL_LOG_INFO("Testing assertions that fail...");

  int value = 0;

  IRSOL_LOG_INFO("Triggering Debug-level assertion failure. This should cause a warning message to "
                 "be printed to the console.");
  IRSOL_ASSERT_DEBUG(value != 0, "Debug assertion failed: value should not be zero.");

  IRSOL_LOG_INFO("Triggering Error-level assertion failure. This should cause an error message to "
                 "be printed to the console, and an irsol::AssertionException to be fired.");
  try {
    IRSOL_ASSERT_ERROR(value != 0, "Error assertion failed: value should not be zero");
  } catch(const irsol::AssertionException& ex) {
    IRSOL_LOG_ERROR("Caught AssertionException: {}", ex.what());
  }

  IRSOL_LOG_INFO("Continuing after catching Error-level assertion.");

  IRSOL_LOG_INFO("Triggering Fatal-level assertion failure (program will terminate)...");
  // Note: This will terminate the program. Uncomment to test.
  // IRSOL_ASSERT_FATAL(value != 0, "Fatal assertion failed: value should not be zero");

  IRSOL_LOG_INFO("This message will not be printed if Fatal assertion is triggered.");
}

int
main()
{
  // Construct log file path based on program name
  std::string logPath = "logs/" + getProgramName() + ".log";
  // Initialize logging and assertion systems
  irsol::initLogging(logPath.c_str());
  irsol::initAssertHandler();

  IRSOL_LOG_INFO("Starting example application");

  demoLogging();
  demoLoggingWithArguments();

  demoPassingAssertions();
  demoFailingAssertions();

  return 0;
}
