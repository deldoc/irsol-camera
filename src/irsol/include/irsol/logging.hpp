#pragma once

#include "spdlog/spdlog.h"
#include <chrono>
#include <unordered_map>

#define IRSOL_LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define IRSOL_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define IRSOL_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define IRSOL_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define IRSOL_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define IRSOL_LOG_FATAL(...) spdlog::critical(__VA_ARGS__)

namespace irsol {
namespace internal {
class NamedLoggerRegistry {
public:
  static spdlog::logger *getLogger(const std::string &name) {
    spdlog::logger *result;
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) {
      result = it->second.logger.get();
      it->second.lastRetrieved = std::chrono::system_clock::now();
    } else {
      auto newLogger = spdlog::default_logger()->clone(name);
      LoggerInfo info = {newLogger, std::chrono::system_clock::now()};
      m_loggers[name] = info;
      result = newLogger.get();
    }

    // If there's more than 256 loggers, delete the oldest one
    if (m_loggers.size() > 256) {
      IRSOL_LOG_INFO("Automatic deletion of old named loggers.");
      auto oldestLogger =
          std::min_element(m_loggers.begin(), m_loggers.end(), [](const auto &a, const auto &b) {
            return a.second.lastRetrieved < b.second.lastRetrieved;
          });
      m_loggers.erase(oldestLogger);
    }
    return result;
  }

private:
  struct LoggerInfo {
    std::shared_ptr<spdlog::logger> logger;
    std::chrono::time_point<std::chrono::system_clock> lastRetrieved;
  };
  static std::unordered_map<std::string, LoggerInfo> m_loggers;
};
} // namespace internal
} // namespace irsol

#define IRSOL_NAMED_LOG_TRACE(name, ...)                                                           \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->trace(__VA_ARGS__)
#define IRSOL_NAMED_LOG_DEBUG(name, ...)                                                           \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->debug(__VA_ARGS__)
#define IRSOL_NAMED_LOG_INFO(name, ...)                                                            \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->info(__VA_ARGS__)
#define IRSOL_NAMED_LOG_WARN(name, ...)                                                            \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->warn(__VA_ARGS__)
#define IRSOL_NAMED_LOG_ERROR(name, ...)                                                           \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->error(__VA_ARGS__)
#define IRSOL_NAMED_LOG_FATAL(name, ...)                                                           \
  irsol::internal::NamedLoggerRegistry::getLogger(name)->critical(__VA_ARGS__)

namespace irsol {
void initLogging(const char *fileSinkFilename = "log/irsol.log");
} // namespace irsol