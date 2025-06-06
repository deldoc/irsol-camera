#include "irsol/logging.hpp"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace irsol {
namespace internal {
std::unordered_map<std::string, LoggerInfo> NamedLoggerRegistry::m_loggers;

spdlog::logger*
NamedLoggerRegistry::getLogger(const std::string& name)
{
  spdlog::logger* result;
  auto            it = m_loggers.find(name);
  if(it != m_loggers.end()) {
    result                   = it->second.logger.get();
    it->second.lastRetrieved = irsol::types::clock_t::now();
  } else {
    auto newLogger = spdlog::default_logger()->clone(name);

#ifdef DEBUG
    // Extract the current logFilePath from the default logger
    auto& all_sinks = newLogger->sinks();
    // Find a logger of type rotating_file_sink_mt and determine the logFilePath
    // for the named logger.
    bool        fileSinkFound = false;
    std::string newLogFilePath;
    for(auto& existing_sink : all_sinks) {
      if(
        spdlog::sinks::rotating_file_sink_mt* rotating_sink =
          dynamic_cast<spdlog::sinks::rotating_file_sink_mt*>(existing_sink.get())) {
        std::string logFilePath = rotating_sink->filename();
        // strip the suffix from the logFilePath
        size_t pos = logFilePath.rfind('.');
        if(pos != std::string::npos) {
          logFilePath = logFilePath.substr(0, pos);
        }
        newLogFilePath = logFilePath + "_" + name + ".log";
        fileSinkFound  = true;
        break;
      }
    }
    if(!fileSinkFound) {
      newLogFilePath = name + ".log";
    }

    // Also create a new named sink for debug mode
    const auto maxFileSize = 1024 * 1024 * 5;  // 5 MB
    const auto maxFiles    = 24;               // Keep 24 rotated files
    auto       fileSink    = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
      newLogFilePath, maxFileSize, maxFiles, false);

    // Add the dedicated file sink to the named logger
    newLogger->sinks().push_back(fileSink);
#endif
    LoggerInfo info = {newLogger, irsol::types::clock_t::now()};
    m_loggers[name] = info;
    result          = newLogger.get();
  }

  // If there's more than 256 loggers, delete the oldest one
  if(m_loggers.size() > 256) {
    IRSOL_LOG_INFO("Automatic deletion of old named loggers.");
    auto oldestLogger =
      std::min_element(m_loggers.begin(), m_loggers.end(), [](const auto& a, const auto& b) {
        return a.second.lastRetrieved < b.second.lastRetrieved;
      });
    m_loggers.erase(oldestLogger);
  }
  return result;
}

}  // namespace internal

void
setLoggerName(const char* name)
{
  auto logger = spdlog::default_logger()->clone(name);
  spdlog::set_default_logger(logger);
}

void
setLoggingFormat(LoggingFormat format, std::optional<std::shared_ptr<spdlog::logger>> logger)
{
  auto thisLogger = logger.value_or(spdlog::default_logger());
  for(auto& sink : thisLogger->sinks()) {
    setSinkLoggingFormat(format, sink);
  }
}
void
setSinkLoggingFormat(LoggingFormat format, std::shared_ptr<spdlog::sinks::sink> sink)
{
  switch(format) {
    case LoggingFormat::FILE:
      sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][%n][pid %P][tid %t][%s:%!():%#] %v");
      break;
    case LoggingFormat::CONSOLE:
      sink->set_pattern("[%H:%M:%S.%e][%^%l%$][%n][%s:%!():%#] %v");
      break;
    case LoggingFormat::UNIT_TESTS:
      sink->set_pattern("[%^%l%$][%s:%!():%#] %v");
      break;
  }
}

void
initLogging(const char* logFilePath, std::optional<spdlog::level::level_enum> minLogLevel)
{
#ifdef DEBUG
  // Set the logging level to debug if in debug mode
  const auto defaultConsoleLevel  = spdlog::level::trace;
  const auto defaultFileSinkLevel = spdlog::level::trace;
#else
  // Set the logging level to info if not in debug mode
  const auto defaultConsoleLevel  = spdlog::level::info;
  const auto defaultFileSinkLevel = spdlog::level::info;
#endif

  auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  setSinkLoggingFormat(LoggingFormat::CONSOLE, consoleSink);

  const auto maxFileSize = 1024 * 1024 * 5;  // 5 MB
  const auto maxFiles    = 24;               // Keep 24 rotated files
  auto       fileSink    = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
    logFilePath, maxFileSize, maxFiles, false);
  setSinkLoggingFormat(LoggingFormat::FILE, fileSink);

  // Set the loggers as default loggers
  auto logger =
    std::make_shared<spdlog::logger>("irsol", spdlog::sinks_init_list{consoleSink, fileSink});
  spdlog::set_default_logger(logger);

#ifdef DEBUG
  // Force flush on trace level and above
  spdlog::flush_on(spdlog::level::trace);
#else
  // Force flush on error level and above
  spdlog::flush_on(spdlog::level::err);
#endif

  // Force flush every N seconds
  spdlog::flush_every(std::chrono::seconds(2));

  // Configure the levels for logging on the different sinks
  auto consoleLevel  = defaultConsoleLevel;
  auto fileSinkLevel = defaultFileSinkLevel;
  if(minLogLevel.has_value()) {
    // Override the console and file levels if needed
    consoleLevel  = std::max({defaultConsoleLevel, *minLogLevel});
    fileSinkLevel = std::max({defaultFileSinkLevel, *minLogLevel});
  }
  auto globalLevel = std::min({consoleLevel, fileSinkLevel});

  consoleSink->set_level(consoleLevel);
  fileSink->set_level(fileSinkLevel);
  logger->set_level(globalLevel);

#ifdef DEBUG
  spdlog::info("Logging initialized with sync levels");
  spdlog::info("Console {}", spdlog::level::to_string_view(consoleLevel));
  spdlog::info("File: {}", spdlog::level::to_string_view(fileSinkLevel));
  if(minLogLevel.has_value()) {
    spdlog::info("Global: {} (overridden)", spdlog::level::to_string_view(minLogLevel.value()));
  } else {
    spdlog::info("Global: {}", spdlog::level::to_string_view(globalLevel));
  }
#endif
}
}  // namespace irsol