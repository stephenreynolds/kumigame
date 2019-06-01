#include "game.hpp"
#include "debug/glDebug.hpp"
#include "debug/log.hpp"
#include "input/keyboard.hpp"
#include "settings.hpp"
#include "shader.hpp"
#include <fmt/format.h>
#include <glad/glad.h>

Game::Game()
{
}

Game::~Game()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

Result Game::run()
{
    if (!init() || !loadAssets())
    {
        return RESULT_FAILURE;
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        processInput();
        update();
        draw();
    }

    return RESULT_SUCCESS;
}

Result Game::init()
{
    LOG_INFO("Version: {}", VERSION.toLongString());

    readSettings(settings, "settings.toml");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwSetErrorCallback([](int error, const char* description) {
        LOG_ERROR("GLFW error (code {}): {}", error, description);
    });

    window = glfwCreateWindow(settings.width, settings.height, TITLE, nullptr, nullptr);
    if (!window)
    {
        const char* what;
        glfwGetError(&what);
        LOG_ERROR("Failed to create window: {}", what);
        return RESULT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        LOG_ERROR("Failed to initialize GLAD.");
        return RESULT_FAILURE;
    }

    GLint flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    return RESULT_SUCCESS;
}

Result Game::loadAssets()
{
    // Load shaders.
    auto textShader = std::make_shared<Shader>("assets/shaders/text.vert", "assets/shaders/text.frag", "text");

    Shader("assets/shaders/cube.vert", "assets/shaders/cube.frag", "cube");

    textRenderer = std::make_shared<TextRenderer>(settings.width, settings.height, textShader);

    statsViewer = std::make_unique<StatsViewer>(textRenderer, glm::vec2(20.0f));
    statsViewer->loadAssets();

    // Cube
    float vertices[] = {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return RESULT_SUCCESS;
}

void Game::processInput()
{
    Keyboard::processKeys(window);

    if (Keyboard::pressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    statsViewer->processInput(window);
}

void Game::update()
{
    statsViewer->update();
}

void Game::draw()
{
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader::get("cube").use();

    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    statsViewer->draw(VERSION.toLongString(), settings.width); // Draw stats last so it appears on top.

    glfwSwapBuffers(window);
}
