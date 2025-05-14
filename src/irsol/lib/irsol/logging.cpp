#include "irsol/logging.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace irsol {
void init_logging(const char *log_file_path) {
#ifdef DEBUG
  // Set the logging level to debug if in debug mode
  const auto console_sink_level = spdlog::level::debug;
  const auto file_sink_level = spdlog::level::trace;
#else
  // Set the logging level to info if not in debug mode
  const auto console_sink_level = spdlog::level::info;
  const auto file_sink_level = spdlog::level::info;
#endif
  const auto global_level = std::min({console_sink_level, file_sink_level});

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(console_sink_level);
  console_sink->set_pattern("[%^%l%$] %v");

  const auto max_file_size = 1024 * 1024 * 5; // 5 MB
  const auto max_files = 24;                  // Keep 24 rotated files
  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      log_file_path, max_file_size, max_files, false);
  file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
  file_sink->set_level(file_sink_level);

  // Set the loggers as default loggers
  auto logger =
      std::make_shared<spdlog::logger>("irsol", spdlog::sinks_init_list{console_sink, file_sink});
  logger->set_level(global_level);
  spdlog::set_default_logger(logger);
}
} // namespace irsol