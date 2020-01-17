#ifndef KUMIGAME_GAME_HPP
#define KUMIGAME_GAME_HPP

#include "camera.hpp"
#include "settings.hpp"
#include "version.hpp"
#include "debug/statsViewer.hpp"
#include "renderer/textRenderer.hpp"
#include <GLFW/glfw3.h>
#include <optional>
#include <renderer/texture.hpp>

class Game
{
public:
    Game();

    ~Game();

    std::optional<std::string> run();

private:
    const char *TITLE = "kumigame";
    const Version VERSION = Version(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH); // VERSION_X defined at compile-time.
    GLFWwindow *window = nullptr;
    Settings settings;
    std::unique_ptr<Camera> camera;
    std::shared_ptr<TextRenderer> textRenderer;
    std::unique_ptr<StatsViewer> statsViewer;
    std::unique_ptr<Shader> spriteShader;
    unsigned int vao = 0;
    unsigned int vbo = 0;
    std::unique_ptr<Texture2D> texture1;
    std::unique_ptr<Texture2D> texture2;

    std::optional<std::string> init();
    std::optional<std::string> loadAssets();
    void processInput(float deltaTime);
    void update();
    void draw();
};

#endif //KUMIGAME_GAME_HPP
