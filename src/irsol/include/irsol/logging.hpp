/**
 * @file irsol/logging.hpp
 * @brief Logging utilities and configuration for the irsol library.
 *
 * Provides macros and functions for consistent and configurable logging using spdlog.
 * Supports named loggers, different output formats, and compile-time level control.
 *
 * This logging interface is built on top of the [spdlog](https://github.com/gabime/spdlog) library.
 *
 * @ingroup Logging
 */

#pragma once

#include "irsol/types.hpp"

#include <optional>

// Compile-time log-level (de-)activation
#ifndef SPDLOG_ACTIVE_LEVEL
#ifdef DEBUG
// If not provided externally, if running in debug mode, we allow logging all levels
#define SPDLOG_ACTIVE_LEVEL 0  // trace
#else
// If not provided externally, if running in release mode, we only allow logging to info level and
// never lower (debug and trace)
#define SPDLOG_ACTIVE_LEVEL 2  // info
#endif
#endif

#include <spdlog/spdlog.h>
#include <unordered_map>

/**
 * @defgroup Logging Logging
 * @brief Logging macros, types, and functions for the irsol library.
 *
 * This group contains macros and helper utilities to log messages with different severity levels,
 * configure logger output formats, and handle named loggers.
 */

/**
 * @def SPDLOG_ACTIVE_LEVEL
 * @brief Controls the compile-time minimum logging level.
 *
 * If not defined externally, this header sets it to:
 * - `SPDLOG_LEVEL_TRACE` (0) when compiled with DEBUG mode.
 * - `SPDLOG_LEVEL_INFO`  (2) when compiled in release mode.
 *
 * All logging macros that correspond to a level lower than the one defined by the
 * `SPDLOG_ACTIVE_LEVEL` are disabled at compile time.
 */

/**
 * @def IRSOL_LOG_TRACE
 * @ingroup Logging
 * @brief Logs a trace-level message using the default logger.
 */

/**
 * @def IRSOL_LOG_DEBUG
 * @ingroup Logging
 * @brief Logs a debug-level message using the default logger.
 */

/**
 * @def IRSOL_LOG_INFO
 * @ingroup Logging
 * @brief Logs an info-level message using the default logger.
 */

/**
 * @def IRSOL_LOG_WARN
 * @ingroup Logging
 * @brief Logs a warning-level message using the default logger.
 */

/**
 * @def IRSOL_LOG_ERROR
 * @ingroup Logging
 * @brief Logs an error-level message using the default logger.
 */

/**
 * @def IRSOL_LOG_FATAL
 * @ingroup Logging
 * @brief Logs a fatal (critical) message using the default logger.
 */

#define IRSOL_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define IRSOL_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define IRSOL_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define IRSOL_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define IRSOL_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define IRSOL_LOG_FATAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

namespace irsol {
namespace internal {

/**
 * @ingroup Logging
 * @brief Metadata for a named logger instance.
 */
struct LoggerInfo
{
  std::shared_ptr<spdlog::logger> logger;         ///< The spdlog logger instance.
  irsol::types::timepoint_t       lastRetrieved;  ///< Last time this logger was accessed.
};

/**
 * @ingroup Logging
 * @brief Manages a registry of named loggers.
 *
 * Provides access to named logger instances and stores metadata for reuse and caching.
 */
class NamedLoggerRegistry
{
public:
  /**
   * @brief Retrieves a logger by name from the registry.
   *
   * If the logger does not exist yet, it is created and registered.
   *
   * @param name The name of the logger.
   * @return Pointer to the associated spdlog logger.
   */
  static spdlog::logger* getLogger(const std::string& name);

private:
  static std::unordered_map<std::string, LoggerInfo> m_loggers;  ///< Internal logger registry.
};

}  // namespace internal
}  // namespace irsol

/**
 * @def IRSOL_NAMED_LOG_TRACE
 * @ingroup Logging
 * @brief Logs a trace-level message using a named logger.
 */

/**
 * @def IRSOL_NAMED_LOG_DEBUG
 * @ingroup Logging
 * @brief Logs a debug-level message using a named logger.
 */

/**
 * @def IRSOL_NAMED_LOG_INFO
 * @ingroup Logging
 * @brief Logs an info-level message using a named logger.
 */

/**
 * @def IRSOL_NAMED_LOG_WARN
 * @ingroup Logging
 * @brief Logs a warning-level message using a named logger.
 */

/**
 * @def IRSOL_NAMED_LOG_ERROR
 * @ingroup Logging
 * @brief Logs an error-level message using a named logger.
 */

/**
 * @def IRSOL_NAMED_LOG_FATAL
 * @ingroup Logging
 * @brief Logs a fatal (critical) message using a named logger.
 */

#define IRSOL_NAMED_LOG_TRACE(name, ...) \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->trace(__VA_ARGS__)
#define IRSOL_NAMED_LOG_DEBUG(name, ...) \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->debug(__VA_ARGS__)
#define IRSOL_NAMED_LOG_INFO(name, ...) \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->info(__VA_ARGS__)
#define IRSOL_NAMED_LOG_WARN(name, ...) \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->warn(__VA_ARGS__)
#define IRSOL_NAMED_LOG_ERROR(name, ...) \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->error(__VA_ARGS__)
#define IRSOL_NAMED_LOG_FATAL(name, ...) \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->critical(__VA_ARGS__)

namespace irsol {

/**
 * @ingroup Logging
 * @brief Enum representing supported logging output formats.
 */
enum class LoggingFormat
{
  CONSOLE,    ///< Human-readable format for terminal output.
  FILE,       ///< Persistent file logging format.
  UNIT_TESTS  ///< Format suitable for unit test frameworks.
};

/**
 * @ingroup Logging
 * @brief Sets the global logging format.
 *
 * This function configures the default output format (e.g., console, file).
 *
 * @param format The desired output format.
 * @param logger Optional logger instance to configure, if not provided the format is set to default
 * logger.
 */
void setLoggingFormat(
  LoggingFormat                                  format = LoggingFormat::FILE,
  std::optional<std::shared_ptr<spdlog::logger>> logger = std::nullopt);

/**
 * @ingroup Logging
 * @brief Sets the logging format for a specific sink.
 *
 * Applies the appropriate formatter string to the given sink (e.g., console, file).
 *
 * @param format The desired output format.
 * @param sink The sink to configure.
 */
void setSinkLoggingFormat(LoggingFormat format, std::shared_ptr<spdlog::sinks::sink> sink);

/**
 * @ingroup Logging
 * @brief Sets the name of the default logger.
 *
 * Use this to label logs with a custom identifier. Useful for distinguishing between
 * multiple libraries or subsystems.
 *
 * @param name The desired logger name.
 */
void setLoggerName(const char* name);

/**
 * @ingroup Logging
 * @brief Initializes the `irsol` logging system.
 *
 * Creates and configures the global logger instance. This should typically be called
 * once at application startup.
 *
 * @param fileSinkFilename Path to the log file where logs are written to.
 * @param minLogLevel Optional minimum log level to use at runtime.
 */
void initLogging(
  const char*                              fileSinkFilename = "log/irsol.log",
  std::optional<spdlog::level::level_enum> minLogLevel      = std::nullopt);

/**
 * @ingroup Logging
 * @brief Runtime map from string names to spdlog log levels.
 *
 * Allows dynamic parsing of log levels from configuration or user input.
 */
inline const std::unordered_map<std::string, spdlog::level::level_enum> levelNameToLevelMap = {
#ifdef DEBUG
  {"trace", spdlog::level::trace},
  {"debug", spdlog::level::debug},
#endif
  {"info", spdlog::level::info},
  {"warn", spdlog::level::warn},
  {"error", spdlog::level::err}};
}  // namespace irsol
