#include "settings.hpp"
#include "debug/log.hpp"
#include <cpptoml.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

void readSettings(Settings &settings, const char *filepath)
{
    try
    {
        std::ifstream file(filepath);

        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        auto tomlSettings = cpptoml::parse_file(filepath);

        settings.width = tomlSettings->get_qualified_as<int>("graphics.display.width").value_or(settings.width);
        settings.height = tomlSettings->get_qualified_as<int>("graphics.display.height").value_or(settings.height);
        settings.fullscreen = tomlSettings->get_qualified_as<bool>("graphics.display.fullscreen").value_or(settings.fullscreen);
        settings.vSync = tomlSettings->get_qualified_as<bool>("graphics.display.vSync").value_or(settings.vSync);
        settings.doubleBuffer = tomlSettings->get_qualified_as<bool>("graphics.display.doubleBuffer").value_or(settings.doubleBuffer);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR("Failed to load settings from {}: {}", filepath, e.what());
    }
}
