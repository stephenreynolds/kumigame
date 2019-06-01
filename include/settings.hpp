#ifndef KUMIGAME_SETTINGS_HPP
#define KUMIGAME_SETTINGS_HPP

struct Settings
{
    int width = 1280;
    int height = 720;
    bool fullscreen = false;
};

void readSettings(Settings &settings, const char *filepath);

#endif
