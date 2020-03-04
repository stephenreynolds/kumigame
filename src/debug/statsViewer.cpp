#include "statsViewer.hpp"
#include "debugConsole.hpp"
#include "../input/keyboard.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <fmt/format.h>

StatsViewer::StatsViewer(std::shared_ptr<TextRenderer>& textRenderer, glm::vec2 position, float sampleTime)
    : position(position), sampleTime(sampleTime),
      renderer(textRenderer), lastTime(static_cast<float>(glfwGetTime()))
{
    // Toggle stats visibility.
    Keyboard::addKeyBinding([this]() {
        toggleHidden();
    }, GLFW_KEY_F3, GLFW_RELEASE);

    // Toggle polygon mode (wireframe -> point -> fill).
    Keyboard::addKeyBinding([this]() {
        togglePolygonMode();
    }, GLFW_KEY_F4, GLFW_RELEASE);
}

void StatsViewer::processInput()
{
    if (!DebugConsole::command.processed)
    {
        if (DebugConsole::command.size() == 2)
        {
            if (DebugConsole::command[0] == "toggle")
            {
                if (DebugConsole::command[1] == "stats")
                {
                    toggleHidden();
                    DebugConsole::command.processed = true;
                    DebugConsole::command.response = fmt::format("Toggled stats {}.", hidden ? "off" : "on");
                }
                else if (DebugConsole::command[1] == "line" || DebugConsole::command[1] == "wireframe")
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    DebugConsole::command.processed = true;
                    DebugConsole::command.response = "Set fill mode to \"line\".";
                }
                else if (DebugConsole::command[1] == "point")
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                    DebugConsole::command.processed = true;
                    DebugConsole::command.response = "Set fill mode to \"point\".";
                }
                else if (DebugConsole::command[1] == "fill")
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    DebugConsole::command.processed = true;
                    DebugConsole::command.response = "Set fill mode to \"fill\".";
                }
            }
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

void StatsViewer::render(const std::string& version, glm::ivec2 frameSize, glm::ivec2 renderSize)
{
    // Update every one second.
    if (!hidden)
    {
        glDisable(GL_DEPTH_TEST); // Keep text on top.

        // Prevent text from being drawn as wireframe/points.
        int polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Draw FPS and ms/frame.
        auto out = fmt::format("{0:.0f} ({1:.2f}ms)\n{2}\nWindow: {3}x{4}\nRendering: {5}x{6}",
            fps, ms,
            glGetString(GL_RENDERER),
            frameSize.x, frameSize.y,
            renderSize.x, renderSize.y);
        renderer->render(out, glm::vec2(position.x, position.y), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 0.7f));

        // Draw version.
        out = fmt::format("{}\nOpenGL {}.{}", version, GLVersion.major, GLVersion.minor);
        renderer->render(out, glm::vec2(frameSize.x - 20, 20), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 0.7f), true);

        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        glEnable(GL_DEPTH_TEST); // Restore depth testing.
    }
}

void StatsViewer::toggleHidden()
{
    hidden = !hidden;
}


void StatsViewer::togglePolygonMode()
{
    if (!hidden)
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
