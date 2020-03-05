#include "settings.hpp"
#include "debug/log.hpp"
#include <spdlog/spdlog.h>
#include <toml11/toml.hpp>
#include <limits>
#include <string>

// TODO: Add more checks for valid settings.
void readSettings(Settings &settings, const char *filepath)
{
    try
    {
        settings.file = toml::parse<toml::preserve_comments>(filepath);

        // [graphics.display]
        auto graphicsDisplay = toml::find(settings.file, "graphics", "display");
        settings.width = toml::find_or<int>(graphicsDisplay, "width", static_cast<int>(settings.width));
        settings.height = toml::find_or<int>(graphicsDisplay, "height", static_cast<int>(settings.height));
        settings.fullscreen = toml::find_or<bool>(graphicsDisplay, "fullscreen", settings.fullscreen);
        settings.vSync = toml::find_or<bool>(graphicsDisplay, "vSync", settings.vSync);
        settings.fov = toml::find_or<float>(graphicsDisplay, "fov", static_cast<float>(settings.fov));
        settings.superSampling = toml::find_or<float>(graphicsDisplay, "superSampling", static_cast<float>(settings.superSampling));

        // [log.level]
        auto logLevel = toml::find(settings.file, "log", "level");
        int consoleLevel = toml::find_or<int>(logLevel, "console", settings.consoleLogLevel);
        int fileLevel = toml::find_or<int>(logLevel, "file", settings.fileLogLevel);

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
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed to load settings from {}: {}", filepath, e.what());
    }
}

void saveSettings(Settings &settings, const char *filepath)
{
    try
    {
        // [graphics.display]
        toml::value& graphicsDisplay = toml::find(settings.file, "graphics", "display");
        toml::find(graphicsDisplay, "width") = settings.width;
        toml::find(graphicsDisplay, "height") = settings.height;
        toml::find(graphicsDisplay, "fullscreen") = settings.fullscreen;
        toml::find(graphicsDisplay, "vSync") = settings.vSync;
        toml::find(graphicsDisplay, "fov") = settings.fov;

        // [log.level]
        toml::value& logLevel = toml::find(settings.file, "log", "level");
        toml::find(logLevel, "console") = static_cast<int>(settings.consoleLogLevel);
        toml::find(logLevel, "file") = static_cast<int>(settings.fileLogLevel);

        // Output to file.
        std::ofstream file(filepath);
        file << toml::format(settings.file, 0, std::numeric_limits<float>::digits10, false, true);
        file.close();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Failed to write settings to {}: {}", filepath, e.what());
    }
}
