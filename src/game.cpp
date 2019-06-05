#include "game.hpp"
#include "debug/glDebug.hpp"
#include "debug/log.hpp"
#include "input/keyboard.hpp"
#include "settings.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <fmt/format.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

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

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();

        processInput(deltaTime);
        update(deltaTime);
        draw(deltaTime);
    }

    return RESULT_SUCCESS;
}

Result Game::init()
{
    LOG_INFO("Version: {}", VERSION.toLongString());

    readSettings(settings, "settings.toml");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    glfwSetWindowUserPointer(window, this);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        LOG_ERROR("Failed to initialize GLAD.");
        return RESULT_FAILURE;
    }

    LOG_INFO("OpenGL {}.{}", GLVersion.major, GLVersion.minor);
    LOG_INFO("Graphics device: {}", glGetString(GL_RENDERER));

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

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
    camera->setMaxFOV(settings.fov);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    camera->lastX = width / 2.0f;
    camera->lastY = height / 2.0f;

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
        static_cast<Game*>(glfwGetWindowUserPointer(window))->camera->processMouseMovement(xPos, yPos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
        static_cast<Game*>(glfwGetWindowUserPointer(window))->camera->processMouseScroll(yOffset);
    });

    glfwSwapInterval(settings.vSync ? 1 : 0);

    glEnable(GL_DEPTH_TEST);

    return RESULT_SUCCESS;
}

Result Game::loadAssets()
{
    // Load shaders.
    auto textShader = std::make_shared<Shader>("assets/shaders/text.vert", "assets/shaders/text.frag", "text");

    Shader("assets/shaders/mesh.vert", "assets/shaders/mesh.frag", "mesh");

    // Load textures.
    texture = new Texture2D("container", "assets/textures/container.jpg");

    textRenderer = std::make_shared<TextRenderer>(settings.width, settings.height, textShader);

    statsViewer = std::make_unique<StatsViewer>(textRenderer, glm::vec2(20.0f));
    statsViewer->loadAssets();

    // Cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return RESULT_SUCCESS;
}

void Game::processInput(float deltaTime)
{
    Keyboard::processKeys(window);

    if (Keyboard::pressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (Keyboard::pressed(GLFW_KEY_W))
        camera->processKeyboard(Direction::Forward, deltaTime);
    if (Keyboard::pressed(GLFW_KEY_S))
        camera->processKeyboard(Direction::Backward, deltaTime);
    if (Keyboard::pressed(GLFW_KEY_A))
        camera->processKeyboard(Direction::Left, deltaTime);
    if (Keyboard::pressed(GLFW_KEY_D))
        camera->processKeyboard(Direction::Right, deltaTime);

    statsViewer->processInput(window);
}

void Game::update(float deltaTime)
{
    statsViewer->update();
}

void Game::draw(float deltaTime)
{
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shader = Shader::get("mesh");
    shader.use();

    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    int screenWidth, screenHeight;
    glfwGetWindowSize(window, &screenWidth, &screenHeight);
    glm::mat4 projection = glm::perspective(
        glm::radians(camera->fov), static_cast<float>(screenWidth) / screenHeight, 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glBindVertexArray(vao);
    for (unsigned int i = 0; i < 10; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        glm::mat4 mvp = projection * view * model;
        shader.setMatrix4("mvp", mvp);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    statsViewer->draw(VERSION.toLongString(), settings.width);

    glfwSwapBuffers(window);
}
