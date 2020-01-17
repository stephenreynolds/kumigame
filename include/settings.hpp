#ifndef KUMIGAME_SETTINGS_HPP
#define KUMIGAME_SETTINGS_HPP

#include <spdlog/spdlog.h>

struct Settings
{
    // Display
    int width = 1280;
    int height = 720;
    bool fullscreen = false;
    bool vSync = false;
    float fov = 80.0f;

    // Log
    spdlog::level::level_enum consoleLogLevel = spdlog::level::critical;
    spdlog::level::level_enum fileLogLevel = spdlog::level::warn;
};

void readSettings(Settings &settings, const char *filepath);

#endif //KUMIGAME_SETTINGS_HPP
