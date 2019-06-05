#ifndef KUMIGAME_GAME_HPP
#define KUMIGAME_GAME_HPP

#include "camera.hpp"
#include "result.hpp"
#include "settings.hpp"
#include "debug/statsViewer.hpp"
#include "textRenderer.hpp"
#include "texture.hpp"
#include "version.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

class Game
{
public:
    std::unique_ptr<Camera> camera;

    Game();
    ~Game();

    Result run();

private:
    const char* TITLE = "kumigame";
    const Version VERSION = Version(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    GLFWwindow* window = nullptr;
    Settings settings;
    std::shared_ptr<TextRenderer> textRenderer;
    std::unique_ptr<StatsViewer> statsViewer;

    Texture2D* texture = nullptr;
    GLuint vao;

    Result init();
    Result loadAssets();
    void processInput(float deltaTime);
    void update(float deltaTime);
    void draw(float deltaTime);
};

#endif
