#ifndef KUMIGAME_DEBUG_STATS_VIEWER_HPP
#define KUMIGAME_DEBUG_STATS_VIEWER_HPP

#include "textRenderer.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

class StatsViewer
{
public:
    glm::vec2 position;
    bool hidden = true;

    StatsViewer(std::shared_ptr<TextRenderer>, glm::vec2 position);

    void loadAssets();
    void processInput(GLFWwindow* window);
    void update();
    void draw(const std::string& version, int frameWidth);

private:
    std::shared_ptr<TextRenderer> renderer;
    int frames = 0;
    double fps = 0;
    double ms = 0;
    double lastTime;
};

#endif
