#ifndef KUMIGAME_DEBUG_LOG_HPP
#define KUMIGAME_DEBUG_LOG_HPP

#include <spdlog/spdlog.h>

#define LOG_TRACE(msg, ...) spdlog::get("logger")->trace(msg, ##__VA_ARGS__)
#define LOG_DEBUG(msg, ...) spdlog::get("logger")->debug(msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) spdlog::get("logger")->info(msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...) spdlog::get("logger")->warn(msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) spdlog::get("logger")->error(msg, ##__VA_ARGS__)
#define LOG_CRITICAL(msg, ...) spdlog::get("logger")->critical(msg, ##__VA_ARGS__)

#define LOG_LEVEL_TRACE spdlog::level::trace
#define LOG_LEVEL_DEBUG spdlog::level::debug
#define LOG_LEVEL_INFO spdlog::level::info
#define LOG_LEVEL_WARN spdlog::level::warn
#define LOG_LEVEL_ERROR spdlog::level::err
#define LOG_LEVEL_CRITICAL spdlog::level::critical

void initLog(spdlog::level::level_enum consoleLevel = spdlog::level::warn,
    spdlog::level::level_enum fileLevel = spdlog::level::info);

#endif
