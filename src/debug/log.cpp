#include "log.hpp"
#include <filesystem>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

#ifndef NDEBUG
#include <spdlog/sinks/msvc_sink.h>
#endif

std::string createLogFiles(std::vector<std::string>& errors, unsigned short int keepNumLogs);
void createLogger(spdlog::level::level_enum consoleLevel, spdlog::level::level_enum fileLevel, const char* filename);

void initLog(spdlog::level::level_enum consoleLevel, spdlog::level::level_enum fileLevel, unsigned short int keepNumLogs)
{
    // Keep list of errors that occur while initializing log.
    std::vector<std::string> errors;

    // Create log file and keep previous three.
    std::string fileName = createLogFiles(errors, keepNumLogs);

    // Initialize the logger.
    createLogger(consoleLevel, fileLevel, fileName.c_str());

    // Log any errors that may have occurred while initializing log.
    for (const std::string& error : errors)
    {
        LOG_ERROR(error);
    }
}

void changeLogLevels(spdlog::level::level_enum consoleLevel, spdlog::level::level_enum fileLevel)
{
    spdlog::drop("logger");
    createLogger(consoleLevel, fileLevel, "logs/game.0.log");
}

std::string createLogFiles(std::vector<std::string>& errors, unsigned short int keepNumLogs)
{
    // Make sure logs/ directory exists.
    std::filesystem::create_directory("logs");

    // Rename old log files, keeping the previous three.
    std::string fileName = fmt::format("logs/game.{}.log", keepNumLogs);
    if (std::filesystem::exists(fileName) && std::remove(fileName.c_str()))
    {
        errors.push_back(fmt::format("Could not remove {}.", fileName));
    }

    for (int i = keepNumLogs - 1; i >= 0; --i)
    {
        fileName = fmt::format("logs/game.{}.log", std::to_string(i));
        std::string newFile = fmt::format("logs/game.{}.log", std::to_string(i + 1));
        if (std::rename(fileName.c_str(), newFile.c_str()) && std::filesystem::exists(fileName))
        {
            errors.push_back(fmt::format("Failed to rename {} to {}.", fileName, newFile));
        }
    }

    return fileName;
}

void createLogger(spdlog::level::level_enum consoleLevel, spdlog::level::level_enum fileLevel, const char* filename)
{
    // Create console log sink.
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(consoleLevel);
    consoleSink->set_pattern("[%^%l%$] %v");

    // Create file log sink.
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
    fileSink->set_level(fileLevel);
    fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

    // Create MSVC debug sink.
#ifndef NDEBUG
    auto msvcSink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    msvcSink->set_level(LOG_LEVEL_DEBUG);
#endif

    // Register console and file sinks to "logger".
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(consoleSink);
    sinks.push_back(fileSink);
    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::debug);
    spdlog::register_logger(logger);
}
