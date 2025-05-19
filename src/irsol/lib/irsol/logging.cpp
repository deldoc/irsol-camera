#include "irsol/logging.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace irsol {
namespace internal {
std::unordered_map<std::string, NamedLoggerRegistry::LoggerInfo> NamedLoggerRegistry::m_loggers;

}
void initLogging(const char *logFilePath) {
#ifdef DEBUG
  // Set the logging level to debug if in debug mode
  const auto consoleSinkLevel = spdlog::level::debug;
  const auto fileSinkLevel = spdlog::level::trace;
#else
  // Set the logging level to info if not in debug mode
  const auto consoleSinkLevel = spdlog::level::info;
  const auto fileSinkLevel = spdlog::level::info;
#endif
  const auto globalLevel = std::min({consoleSinkLevel, fileSinkLevel});

  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  consoleSink->set_level(consoleSinkLevel);
  consoleSink->set_pattern("[%^%l%$][%n][pid %P][tid %t] %v");

  const auto maxFileSize = 1024 * 1024 * 5; // 5 MB
  const auto maxFiles = 24;                 // Keep 24 rotated files
  auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(logFilePath, maxFileSize,
                                                                         maxFiles, false);
  fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%n][%^%l%$][pid %P][tid %t] %v");
  fileSink->set_level(fileSinkLevel);

  // Set the loggers as default loggers
  auto logger =
      std::make_shared<spdlog::logger>("irsol", spdlog::sinks_init_list{consoleSink, fileSink});
  logger->set_level(globalLevel);
  spdlog::set_default_logger(logger);

  // Force flush on error level and above
  spdlog::flush_on(spdlog::level::err);

  // Force flush every N seconds
  spdlog::flush_every(std::chrono::seconds(2));
#ifdef DEBUG
  spdlog::info("Logging initialized with sync levels");
  spdlog::info("Console {}", spdlog::level::to_string_view(consoleSinkLevel));
  spdlog::info("File: {}", spdlog::level::to_string_view(fileSinkLevel));
  spdlog::info("Global: {}", spdlog::level::to_string_view(globalLevel));
#endif
}
} // namespace irsol