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

#define IRSOL_LOG_TRACE(...) SPDLOG_TRACE(__VA_ARGS__)
#define IRSOL_LOG_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define IRSOL_LOG_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define IRSOL_LOG_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define IRSOL_LOG_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define IRSOL_LOG_FATAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

namespace irsol {
namespace internal {
struct LoggerInfo
{
  std::shared_ptr<spdlog::logger> logger;
  irsol::types::timepoint_t       lastRetrieved;
};
class NamedLoggerRegistry
{
public:
  static spdlog::logger* getLogger(const std::string& name);

private:
  static std::unordered_map<std::string, LoggerInfo> m_loggers;
};
}  // namespace internal
}  // namespace irsol

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
enum class LoggingFormat
{
  CONSOLE,
  FILE,
  UNIT_TESTS
};
void setLoggingFormat(
  LoggingFormat                                  format = LoggingFormat::FILE,
  std::optional<std::shared_ptr<spdlog::logger>> logger = std::nullopt);
void setSinkLoggingFormat(LoggingFormat format, std::shared_ptr<spdlog::sinks::sink> sink);
void setLoggerName(const char* name);
void initLogging(
  const char*                              fileSinkFilename = "log/irsol.log",
  std::optional<spdlog::level::level_enum> minLogLevel      = std::nullopt);

inline const std::unordered_map<std::string, spdlog::level::level_enum> levelNameToLevelMap = {
#ifdef DEBUG
  {"trace", spdlog::level::trace},
  {"debug", spdlog::level::debug},
#endif
  {"info", spdlog::level::info},
  {"warn", spdlog::level::warn},
  {"error", spdlog::level::err}};
}  // namespace irsol