#include "game.hpp"
#include "debug/glDebug.hpp"
#include "debug/log.hpp"
#include "input/keyboard.hpp"
#include "renderer/texture.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <optional>

Game::Game() = default;

Game::~Game()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwDestroyWindow(window);
    glfwTerminate();
}

std::optional<std::string> Game::run()
{
    if (auto result = init())
    {
        return fmt::format("Failed to initialize: {}", result.value());
    }

    if (auto result = loadAssets())
    {
        return "Failed to load assets.";
    }

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        auto currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(deltaTime);
        update();
        draw();
    }

    return {};
}

std::optional<std::string> Game::init()
{
    readSettings(settings, "settings.toml");
    changeLogLevels(settings.consoleLogLevel, settings.fileLogLevel);

    LOG_INFO("Version {}", VERSION.toLongString());

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
        return fmt::format("Failed to create window: {}", what);
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        return "Failed to initialize GLAD.";
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

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
        glViewport(0, 0, width, height);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos) {
        static_cast<Game*>(glfwGetWindowUserPointer(window))->camera->processMouseMovement(xPos, yPos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yOffset) {
        static_cast<Game*>(glfwGetWindowUserPointer(window))->camera->processMouseScroll(yOffset);
    });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSwapInterval(settings.vSync ? 1 : 0);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    return {};
}

std::optional<std::string> Game::loadAssets()
{
    // Load shaders.
    auto textShader = std::make_shared<Shader>("assets/shaders/text.vert", "assets/shaders/text.frag");

    // Text renderer.
    textRenderer = std::make_shared<TextRenderer>(settings.width, settings.height, textShader);

    // Stats viewer.
    statsViewer = std::make_unique<StatsViewer>(textRenderer, glm::vec2(20.0f));
    statsViewer->loadAssets();

    spriteShader = std::make_unique<Shader>("assets/shaders/sprite.vert", "assets/shaders/sprite.frag");

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

    glGenVertexArrays(1, &vao);

    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int width, height, numChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("assets/textures/container.jpg", &width, &height, &numChannels, 0);
    texture1 = std::make_unique<Texture2D>();
    texture1->generate(width, height, data);
    stbi_image_free(data);

    data = stbi_load("assets/textures/awesomeface.png", &width, &height, &numChannels, 0);
    texture2 = std::make_unique<Texture2D>();
    texture2->internalFormat = GL_RGBA;
    texture2->imageFormat = GL_RGBA;
    texture2->generate(width, height, data);
    stbi_image_free(data);

    spriteShader->use();
    spriteShader->setInteger("texture1", 0);
    spriteShader->setInteger("texture2", 1);

    camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

    return {};
}

void Game::processInput(float deltaTime)
{
    glfwPollEvents();

    Keyboard::processKeys(window);

    if (Keyboard::pressed(GLFW_KEY_ESCAPE))
    {
        glfwSetWindowShouldClose(window, true);
    }

    // TODO: try using OR to move multiple directions in one call.
    if (Keyboard::pressed(GLFW_KEY_W))
    {
        camera->processKeyboard(CameraDirection::Forward, deltaTime);
    }
    if (Keyboard::pressed(GLFW_KEY_S))
    {
        camera->processKeyboard(CameraDirection::Backward, deltaTime);
    }
    if (Keyboard::pressed(GLFW_KEY_A))
    {
        camera->processKeyboard(CameraDirection::Left, deltaTime);
    }
    if (Keyboard::pressed(GLFW_KEY_D))
    {
        camera->processKeyboard(CameraDirection::Right, deltaTime);
    }
    if (Keyboard::pressed(GLFW_KEY_SPACE))
    {
        camera->processKeyboard(CameraDirection::Up, deltaTime);
    }
    if (Keyboard::pressed(GLFW_KEY_LEFT_SHIFT))
    {
        camera->processKeyboard(CameraDirection::Down, deltaTime);
    }
    if (Keyboard::pressed(GLFW_KEY_LEFT_CONTROL))
    {
        camera->movementSpeed = camera->DEFAULT_MOVEMENT_SPEED * 3.0f;
    }
    else
    {
        camera->movementSpeed = camera->DEFAULT_MOVEMENT_SPEED;
    }

    statsViewer->processInput();
}

void Game::update()
{
    statsViewer->update();
}

void Game::draw()
{
    glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    texture1->bind();
    glActiveTexture(GL_TEXTURE1);
    texture2->bind();

    spriteShader->use();

    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(settings.width) / static_cast<float>(settings.height), 0.1f, 100.0f);

    glm::mat4 view = camera->getViewMatrix();

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(-2.0f, -2.0f, 0.0f),
        glm::vec3(-3.0f, -3.0f, 0.0f),
        glm::vec3(-4.0f, -4.0f, 0.0f),
        glm::vec3(-5.0f, -5.0f, 0.0f),
        glm::vec3(-6.0f, -6.0f, 0.0f),

        glm::vec3(0.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -2.0f, 1.0f),
        glm::vec3(-2.0f, -3.0f, 1.0f),
        glm::vec3(-3.0f, -4.0f, 1.0f),
        glm::vec3(-4.0f, -5.0f, 1.0f),
        glm::vec3(-5.0f, -6.0f, 1.0f),

        glm::vec3(0.0f, -2.0f, 2.0f),
        glm::vec3(-1.0f, -3.0f, 2.0f),
        glm::vec3(-2.0f, -4.0f, 2.0f),
        glm::vec3(-3.0f, -5.0f, 2.0f),
        glm::vec3(-4.0f, -6.0f, 2.0f),

        glm::vec3(0.0f, -3.0f, 3.0f),
        glm::vec3(-1.0f, -4.0f, 3.0f),
        glm::vec3(-2.0f, -5.0f, 3.0f),
        glm::vec3(-3.0f, -6.0f, 3.0f),

        glm::vec3(0.0f, -4.0f, 4.0f),
        glm::vec3(-1.0f, -5.0f, 4.0f),
        glm::vec3(-2.0f, -6.0f, 4.0f),

        glm::vec3(0.0f, -5.0f, 5.0f),
        glm::vec3(-1.0f, -6.0f, 5.0f),

        glm::vec3(0.0f, -6.0f, 6.0f),
    };

    glBindVertexArray(vao);
    for (auto cubePosition : cubePositions)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePosition);
        spriteShader->setMatrix4("mvp", projection * view * model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    statsViewer->draw(VERSION.toLongString(), settings.width);

    glfwSwapBuffers(window);
}
