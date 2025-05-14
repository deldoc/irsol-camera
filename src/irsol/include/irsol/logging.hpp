# include "spdlog/spdlog.h"

#define IRSOL_LOG_TRACE(...) spdlog::trace(__VA_ARGS__)
#define IRSOL_LOG_DEBUG(...) spdlog::debug(__VA_ARGS__)
#define IRSOL_LOG_INFO(...) spdlog::info(__VA_ARGS__)
#define IRSOL_LOG_WARN(...) spdlog::warn(__VA_ARGS__)
#define IRSOL_LOG_ERROR(...) spdlog::error(__VA_ARGS__)
#define IRSOL_LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)
#define IRSOL_LOG_FLUSH() spdlog::flush()

namespace irsol {
void init_logging(const char *file_sink_filename = "log/irsol.log");
}