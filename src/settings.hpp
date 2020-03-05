#ifndef KUMIGAME_SETTINGS_HPP
#define KUMIGAME_SETTINGS_HPP

#include <spdlog/spdlog.h>
#include <toml11/toml.hpp>

struct Settings
{
    toml::value file;

    // Display
    int width = 1280;
    int height = 720;
    bool fullscreen = false;
    bool vSync = false;
    float fov = 80.0f;
    float superSampling = 1.0f;

    // Log
    spdlog::level::level_enum consoleLogLevel = spdlog::level::critical;
    spdlog::level::level_enum fileLogLevel = spdlog::level::warn;
};

void readSettings(Settings &settings, const char* filepath);

void saveSettings(Settings &settings, const char* filepath);

#endif //KUMIGAME_SETTINGS_HPP
