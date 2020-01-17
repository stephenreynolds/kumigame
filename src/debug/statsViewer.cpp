#include "../../include/debug/statsViewer.hpp"
#include "../../include/input/keyboard.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <sstream>
#include <utility>
#include <fmt/format.h>

StatsViewer::StatsViewer(std::shared_ptr<TextRenderer>& textRenderer,
                         glm::vec2 position, float sampleTime)
    : position(position), sampleTime(sampleTime),
    renderer(textRenderer), lastTime(static_cast<float>(glfwGetTime()))
{
}

void StatsViewer::loadAssets()
{
    renderer->load("assets/fonts/OCRAEXT.TTF", 14);
}

void StatsViewer::processInput()
{
    // Toggle stats visibility.
    if (Keyboard::once(GLFW_KEY_F3))
    {
        hidden = !hidden;
    }

    // Toggle polygon mode (wireframe -> point -> fill).
    if (!hidden && Keyboard::once(GLFW_KEY_F4))
    {
        int polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
        switch (polygonMode)
        {
            case GL_FILL:
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case GL_LINE:
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            default:
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
        }
    }
}

void StatsViewer::update()
{
    double currentTime = static_cast<float>(glfwGetTime());
    frames++;
    if (currentTime - lastTime >= sampleTime)
    {
        ms = 100.0f / static_cast<float>(frames);
        fps = 1000.0f / ms;

        frames = 0;
        lastTime += sampleTime;
    }
}

void StatsViewer::draw(const std::string& version, int frameWidth)
{
    // Update every one second.
    if (!hidden)
    {
        // Prevent text from being drawn as wireframe/points.
        int polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Draw FPS and ms/frame.
        auto out = fmt::format("{0:.0f} ({1:.2f}ms)", fps, ms);
        renderer->draw(out, glm::vec2(position.x, position.y), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 0.7f));

        // Draw version.
        renderer->draw(version, glm::vec2(frameWidth - 20, 20), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), true);

        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    }
}
