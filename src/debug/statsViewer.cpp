#include "debug/statsViewer.hpp"
#include "input/keyboard.hpp"
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <iomanip>
#include <memory>
#include <sstream>

StatsViewer::StatsViewer(std::shared_ptr<TextRenderer> textRenderer, glm::vec2 position)
    : position(position), renderer(textRenderer), lastTime(glfwGetTime())
{
}

void StatsViewer::loadAssets()
{
    renderer->load("assets/fonts/OCRAEXT.TTF", 14);
}

void StatsViewer::processInput(GLFWwindow* window)
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
    double currentTime = glfwGetTime();
    frames++;
    if (currentTime - lastTime >= 0.1)
    {
        ms = 100.0 / frames;
        fps = 1000.0 / ms;

        frames = 0;
        lastTime += 0.1;
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
        std::ostringstream out;
        out.precision(2);
        out << std::fixed << fps << " (" << ms << "ms)";
        renderer->draw(out.str(), glm::vec2(position.x, position.y), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 0.7f));

        // Draw version.
        renderer->draw(version, glm::vec2(frameWidth - 20, 20), 1.0f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), true);

        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
    }
}
