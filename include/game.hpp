#ifndef KUMIGAME_GAME_HPP
#define KUMIGAME_GAME_HPP

#include "result.hpp"
#include "settings.hpp"
#include "debug/statsViewer.hpp"
#include "textRenderer.hpp"
#include "version.hpp"
#include <GLFW/glfw3.h>
#include <memory>

class Game
{
public:
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
    GLuint vao;
    GLuint ebo;

    Result init();
    Result loadAssets();
    void processInput();
    void update();
    void draw();
};

#endif
