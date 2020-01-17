#ifndef KUMIGAME_DEBUG_STATS_VIEWER_HPP
#define KUMIGAME_DEBUG_STATS_VIEWER_HPP

#include "../renderer/textRenderer.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

class StatsViewer
{
public:
    glm::vec2 position;
    float sampleTime;
    bool hidden = true;

    StatsViewer(std::shared_ptr<TextRenderer>& textRenderer, glm::vec2 position, float sampleTime = 0.1f);

    void loadAssets();
    void processInput();
    void update();
    void draw(const std::string& version, int frameWidth);

private:
    std::shared_ptr<TextRenderer> renderer;
    int frames = 0;
    float fps = 0;
    float ms = 0;
    float lastTime;
};

#endif
