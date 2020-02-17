#ifndef KUMIGAME_GAME_HPP
#define KUMIGAME_GAME_HPP

#include "camera.hpp"
#include "settings.hpp"
#include "version.hpp"
#include "debug/debugConsole.hpp"
#include "debug/statsViewer.hpp"
#include "renderer/model.hpp"
#include "renderer/textRenderer.hpp"
#include <GLFW/glfw3.h>
#include <optional>
#include <renderer/texture.hpp>

class Game
{
public:
    Game() = default;

    ~Game();

    std::optional<std::string> run();

private:
    const char* TITLE = "kumigame";
    const Version VERSION = Version(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH); // VERSION_X defined at compile-time.
    const char* SETTINGS_PATH = "settings.toml";
    GLFWwindow *window = nullptr;
    glm::ivec2 windowPos{};
    glm::ivec2 windowSize{};
    Settings settings;
    std::unique_ptr<Camera> camera;
    std::shared_ptr<TextRenderer> textRenderer;
    std::unique_ptr<DebugConsole> debugConsole;
    std::unique_ptr<StatsViewer> statsViewer;
    std::shared_ptr<Shader> meshShader;
    std::shared_ptr<Shader> lampShader;
    std::unique_ptr<Model> nanosuit;
    std::unique_ptr<Model> cube;

    std::optional<std::string> init();
    std::optional<std::string> loadAssets();
    void processInput(float deltaTime);
    void update();
    void draw();
};

#endif //KUMIGAME_GAME_HPP
