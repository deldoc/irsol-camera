# 00-logging-and-asserting {#logging_and_asserting}

@see examples/00-logging-and-asserting/main.cpp

The `irsol` library comes packed with a flexible **logging** and **assertion** framework designed to help you build robust applications with clear diagnostics and fail-safe conditions.


## Initialization

Both the **logging** and **assertion** subsystems **must be initialized by the user** at application startup before any logging or assertions are used:

```cpp
#include "irsol/logging.hpp"
#include "irsol/assert.hpp"

int main()
{
    // Initialize logging system
    irsol::initLogging("logs/app.log");

    // Initialize assertion handler
    irsol::initAssertHandler();

    // Your application code here...
}
```
* `initLogging()` configures logging sinks, file outputs, and runtime logging level.
* `initAssertHandler()` sets up the assertion failure callback behavior.


## Logging Levels and Configuration
@see irsol/logging.hpp

### Runtime Logging Level Setup

You can control the logging verbosity at runtime by passing the desired level to `initLogging()`:
```cpp
irsol::initLogging("logs/app.log", spdlog::level::debug);
```

This determines which log messages are emitted during execution. In the above example, ony log levels **higher** than `debug` are emitted (`trace` logs are ignored).

### Compile-Time Logging Level (`SPDLOG_ACTIVE_LEVEL`)

To optimize your binary and exclude logs below a certain level at compile time, define the macro (which needs to be set prior to importing `<spdlog/spdlog>`):
```cpp
#define SPDLOG_ACTIVE_LEVEL 2  // or any other level you want
```

This controls which logging macros (e.g., IRSOL_LOG_DEBUG, IRSOL_LOG_INFO) generate any code at all, reducing runtime overhead.

> Of course, if a log-level is configured to be ignored at compile time, setting a runtime level lower than the compile time level will not have any effect.

By default, the _compile-time logging level_ for building executables in Release and Debug mode are the following:
 * Debug: `trace`. All log levels are considered at compile time
 * Release: `info`. Logs with a lower level are ignored at compilation time

### Named Loggers Usage

The system supports named loggers for modular logging scopes:
```cpp
IRSOL_NAMED_LOG_INFO("this-logger-name", "Log message");
```
 * Use named loggers to separate logs from different components or libraries.
 * Named loggers inherit the global sinks and levels configured during initialization.
 * This allows you to filter or format logs based on source module if needed.
  
### Logger Sinks

The logging system configures multiple sinks by default:
 * Console Sink — outputs logs to the terminal/stdout.
 * File Sink — writes logs to a rotating file (e.g., logs/app.log).

These sinks receive all logs (both from global and named loggers), so logs appear both in console and files simultaneously.
Different sinks are configured to have a different _runtime_ log level by default:

| Compilation mode | Debug   | Release |
| ---------------- | ------- | ------- |
| Console logger   | `debug` | `info`  |
| File logger      | `trace` | `info`  |


## Assertion Levels: Usage and Behavior

@see irsol/assert.hpp

### Disabling Assertions Globally

You can **disable all assertions globally** at compile time by defining the macro:

```cpp
#define IRSOL_DISABLE_ASSERT
```
When `IRSOL_DISABLE_ASSERT` is defined:

  * All assertions are disabled regardless of their level.
  * Assertions generate no code and produce no runtime overhead.
  * Use this option only in production builds where performance is critical and assertions are not needed.