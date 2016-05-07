#ifndef _Logging_H
#define _Logging_H

#include <spdlog/spdlog.h>

#define LOG_LEVEL_DEBUG spdlog::level::debug
#define LOG_LEVEL_INFO spdlog::level::info
#define LOG_LEVEL_WARN spdlog::level::warn
#define LOG_LEVEL_ERROR spdlog::level::error

#define INIT_LOGGING(level) \
    spdlog::stdout_logger_mt("console", true); \
    spdlog::get("console")->set_level(level)

#define SET_LOG_LEVEL(level) spdlog::get("console")->set_level(level)

#define DEBUG(...) spdlog::get("console")->debug(__VA_ARGS__)
#define INFO(...) spdlog::get("console")->info(__VA_ARGS__)
#define WARN(...) spdlog::get("console")->warn(__VA_ARGS__)
#define ERROR(...) spdlog::get("console")->error(__VA_ARGS__)

#endif  // _Logging_H
