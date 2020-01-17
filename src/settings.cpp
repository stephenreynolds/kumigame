#include "settings.hpp"
#include "debug/log.hpp"
#include <cpptoml.h>
#include <spdlog/spdlog.h>
#include <string>

void readSettings(Settings &settings, const char *filepath)
{
    try
    {
        std::ifstream file(filepath);

        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        auto tomlSettings = cpptoml::parse_file(filepath);

        // Display
        settings.width = tomlSettings->get_qualified_as<int>("graphics.display.width").value_or(settings.width);
        settings.height = tomlSettings->get_qualified_as<int>("graphics.display.height").value_or(settings.height);
        settings.fullscreen = tomlSettings->get_qualified_as<bool>("graphics.display.fullscreen").value_or(settings.fullscreen);
        settings.vSync = tomlSettings->get_qualified_as<bool>("graphics.display.vSync").value_or(settings.vSync);
        settings.fov = static_cast<float>(tomlSettings->get_qualified_as<double>("graphics.display.fov").value_or(settings.fov));

        // Log
        int consoleLevel = tomlSettings->get_qualified_as<int>("log.level.console").value_or(settings.consoleLogLevel);
        int fileLevel = tomlSettings->get_qualified_as<int>("log.level.file").value_or(settings.fileLogLevel);

        if (consoleLevel < 0 || consoleLevel > 6)
        {
            LOG_ERROR("Console log level out of range 0 to 6. Check [log.level] in {}. Using default of {}.",
                      filepath, static_cast<int>(settings.consoleLogLevel));
            consoleLevel = settings.consoleLogLevel;
        }
        else
        {
            settings.consoleLogLevel = static_cast<spdlog::level::level_enum>(consoleLevel);
        }

        if (fileLevel < 0 || fileLevel > 6)
        {
            LOG_ERROR("File log level out of range 0 to 6. Check [log.level] in {}. Using default of {}.",
                      filepath, static_cast<int>(settings.fileLogLevel));
            fileLevel = settings.fileLogLevel;
        }
        else
        {
            settings.fileLogLevel = static_cast<spdlog::level::level_enum>(fileLevel);
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed to load settings from {}: {}", filepath, e.what());
    }
}
