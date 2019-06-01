#include "debug/log.hpp"
#include <filesystem>
#include <memory>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

void initLog(spdlog::level::level_enum consoleLevel, spdlog::level::level_enum fileLevel)
{
    // Keep list of errors that occur while initializing log.
    std::vector<std::string> errors;

    // Make sure logs/ directory exists.
    std::filesystem::create_directory("logs");

    // Rename old log files, keeping the previous three.
    std::string filename = "logs/game.3.log";
    if (std::filesystem::exists(filename) && std::remove(filename.c_str()))
    {
        errors.push_back("Could not remove " + filename + ".");
    }
    for (int i = 2; i >= 0; --i)
    {
        filename = "logs/game." + std::to_string(i) + ".log";
        std::string newfile = "logs/game." + std::to_string(i + 1) + ".log";
        if (std::rename(filename.c_str(), newfile.c_str()) && std::filesystem::exists(filename))
        {
            errors.push_back("Failed to rename " + filename + " to " + newfile + ".");
        }
    }

    // Create console log sink.
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(consoleLevel);
    consoleSink->set_pattern("[%^%l%$] %v");

    // Create file log sink.
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
    fileSink->set_level(fileLevel);
    fileSink->set_pattern("[%Y-%m-%d %I:%M:%S.%e %p] [%^%l%$] %v");

    // Register console and file sinks to "logger".
    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(consoleSink);
    sinks.push_back(fileSink);
    auto logger = std::make_shared<spdlog::logger>("logger", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::debug);
    spdlog::register_logger(logger);

    for (std::string error : errors)
    {
        LOG_ERROR(error);
    }
}
