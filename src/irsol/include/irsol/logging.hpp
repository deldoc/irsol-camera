#pragma once

#include "spdlog/spdlog.h"

#define IRSOL_LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define IRSOL_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define IRSOL_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define IRSOL_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define IRSOL_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define IRSOL_LOG_FATAL(...) spdlog::critical(__VA_ARGS__)

namespace irsol {
void initLogging(const char *fileSinkFilename = "log/irsol.log");
} // namespace irsol