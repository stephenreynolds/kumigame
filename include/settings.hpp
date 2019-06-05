#ifndef KUMIGAME_SETTINGS_HPP
#define KUMIGAME_SETTINGS_HPP

struct Settings
{
    int width = 1280;
    int height = 720;
    bool fullscreen = false;
    bool vSync = false;
    float fov = 80.0f;
};

void readSettings(Settings &settings, const char *filepath);

#endif
