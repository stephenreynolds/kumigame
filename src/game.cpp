#include "game.hpp"
#include "debug/glDebug.hpp"
#include "debug/log.hpp"
#include "input/keyboard.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>
#include <stb_image.h>

Game::~Game()
{
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
    readSettings(settings, SETTINGS_PATH);
    changeLogLevels(settings.consoleLogLevel, settings.fileLogLevel);

    LOG_INFO("Version {}", VERSION.toLongString());

    // Initialize GLFW.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); // TODO: Only works on Windows and X11. Find better solution.
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwSetErrorCallback([](int error, const char* description) {
        LOG_ERROR("GLFW error (code {}): {}", error, description);
    });

    // Create window.
    window = glfwCreateWindow(settings.width, settings.height, TITLE, nullptr, nullptr);
    if (!window)
    {
        const char* what;
        glfwGetError(&what);
        return fmt::format("Failed to create window: {}", what);
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

    // Save window position and size.
    glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
    glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

    // Set window icon.
    int width, height, numComponents;
    unsigned char* iconData = stbi_load("assets/images/icon.png", &width, &height, &numComponents, 0);
    GLFWimage icon = {.width = width, .height = height, .pixels = iconData };
    iconData = stbi_load("assets/images/icon_small.png", &width, &height, &numComponents, 0);
    GLFWimage iconSmall = {.width = width, .height = height, .pixels = iconData };
    GLFWimage images[2] = { icon, iconSmall };
    glfwSetWindowIcon(window, 2, images);

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
        auto game = static_cast<Game*>(glfwGetWindowUserPointer(window));
        if (game->debugConsole->hidden)
        {
            game->camera->processMouseMovement(xPos, yPos);
        }
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double, double yOffset) {
        auto game = static_cast<Game*>(glfwGetWindowUserPointer(window));
        if (game->debugConsole->hidden)
        {
            game->camera->processMouseScroll(yOffset);
        }
    });

    glfwSetKeyCallback(window, [](GLFWwindow*, int key, int, int action, int mods) {
        Keyboard::onKeyEvent(key, action, mods);
    });

    glfwSetCharCallback(window, [](GLFWwindow*, unsigned int codePoint) {
        Keyboard::onCharEvent(codePoint);
    });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSwapInterval(settings.vSync ? 1 : 0);

    glEnable(GL_DEPTH_TEST);

    return {};
}

std::optional<std::string> Game::loadAssets()
{
    // Load shaders.
    auto textShader = std::make_shared<Shader>("assets/shaders/text.vert", "assets/shaders/text.frag");
    meshShader = std::make_shared<Shader>("assets/shaders/mesh.vert", "assets/shaders/mesh.frag");
    lampShader = std::make_shared<Shader>("assets/shaders/mesh.vert", "assets/shaders/lamp.frag");

    // Text renderer.
    textRenderer = std::make_shared<TextRenderer>(settings.width, settings.height, textShader, "assets/fonts/OCRAEXT.TTF", 14);

    // Debug console.
    debugConsole = std::make_unique<DebugConsole>(textRenderer, glm::vec2(20.0f, static_cast<float>(settings.height) - 20.0f));

    // Stats viewer.
    statsViewer = std::make_unique<StatsViewer>(textRenderer, glm::vec2(20.0f));

    // Camera
    camera = std::make_unique<Camera>();

    // Load models.
    nanosuit = std::make_unique<Model>("assets/models/nanosuit/nanosuit.obj");
    cube = std::make_unique<Model>("assets/models/cube/cube.obj");

    return {};
}

void Game::processInput(float deltaTime)
{
    glfwPollEvents();

    Keyboard::processKeys(window);

    // Process console commands.
    if (!(DebugConsole::commandProcessed || DebugConsole::command.empty()))
    {
        if (DebugConsole::command.size() == 2)
        {
            if ((DebugConsole::command[0] == "exit" || DebugConsole::command[0] == "close") && DebugConsole::command[1] == "game")
            {
                glfwSetWindowShouldClose(window, true);
                DebugConsole::commandProcessed = true;
                DebugConsole::response = "Exiting.";
            }
            else if (DebugConsole::command[0] == "settings" && DebugConsole::command[1] == "save")
            {
                saveSettings(settings, SETTINGS_PATH);
                DebugConsole::commandProcessed = true;
                DebugConsole::response = fmt::format("Settings saved at \"{}\".", SETTINGS_PATH);
            }
        }
        else if (DebugConsole::command.size() == 3)
        {
            if (DebugConsole::command[0] == "set")
            {
                if (DebugConsole::command[1] == "fullscreen")
                {
                    if (DebugConsole::command[2] == "true")
                    {
                        settings.fullscreen = true;
                        glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
                        glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
                        const auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
                        DebugConsole::response = "Set window mode to fullscreen.";
                    }
                    else if (DebugConsole::command[2] == "false")
                    {
                        settings.fullscreen = false;
                        glfwSetWindowMonitor(window, nullptr, windowPos.x, windowPos.y, windowSize.x, windowSize.y, 0);
                        DebugConsole::response = "Set window mode to windowed.";
                    }

                    DebugConsole::commandProcessed = true;
                }
                else if (DebugConsole::command[1] == "fov")
                {
                    try
                    {
                        float fov = std::stof(DebugConsole::command[2]);
                        camera->fov = fov;
                        settings.fov = fov;
                        DebugConsole::response = fmt::format("Set FOV to {:.1f}.", fov);
                    }
                    catch (std::invalid_argument& ex)
                    {
                        DebugConsole::response = "Invalid argument: must be of type float.";
                    }

                    DebugConsole::commandProcessed = true;
                }
                else if (DebugConsole::command[1] == "vsync")
                {
                    if (DebugConsole::command[2] == "true")
                    {
                        glfwSwapInterval(1);
                        settings.vSync = true;
                        DebugConsole::response = "Vertical sync enabled.";
                    }
                    else if (DebugConsole::command[2] == "false")
                    {
                        glfwSwapInterval(0);
                        settings.vSync = false;
                        DebugConsole::response = "Vertical sync disabled.";
                    }

                    DebugConsole::commandProcessed = true;
                }
            }
        }
        else if (DebugConsole::command.size() == 4)
        {
            if (DebugConsole::command[0] == "set")
            {
                if (DebugConsole::command[1] == "window")
                {
                    try
                    {
                        int width = std::stoi(DebugConsole::command[2]);
                        int height = std::stoi(DebugConsole::command[3]);
                        glfwSetWindowSize(window, width, height);
                        settings.width = width;
                        settings.height = height;
                        DebugConsole::response = fmt::format("Set window dimensions to {0:d}x{1:d}.", width, height);
                    }
                    catch (std::invalid_argument& ex)
                    {
                        DebugConsole::response = "Invalid argument: must be of type int.";
                    }

                    DebugConsole::commandProcessed = true;
                }
            }
        }
    }

    // Close window on Escape.
    Keyboard::addKeyBinding([this]() {
        glfwSetWindowShouldClose(window, true);
    }, GLFW_KEY_ESCAPE, GLFW_RELEASE);

    if (debugConsole->hidden)
    {
        CameraDirection cameraDirection = CameraDirection::None;
        if (Keyboard::pressed(GLFW_KEY_W))
        {
            cameraDirection |= CameraDirection::Forward;
        }
        if (Keyboard::pressed(GLFW_KEY_S))
        {
            cameraDirection |= CameraDirection::Backward;
        }
        if (Keyboard::pressed(GLFW_KEY_A))
        {
            cameraDirection |= CameraDirection::Left;
        }
        if (Keyboard::pressed(GLFW_KEY_D))
        {
            cameraDirection |= CameraDirection::Right;
        }
        if (Keyboard::pressed(GLFW_KEY_SPACE))
        {
            cameraDirection |= CameraDirection::Up;
        }
        if (Keyboard::pressed(GLFW_KEY_C))
        {
            cameraDirection |= CameraDirection::Down;
        }
        if (Keyboard::pressed(GLFW_KEY_LEFT_SHIFT))
        {
            camera->movementSpeed = camera->DEFAULT_MOVEMENT_SPEED * 3.0f;
        }
        else
        {
            camera->movementSpeed = camera->DEFAULT_MOVEMENT_SPEED;
        }

        camera->processKeyboard(cameraDirection, deltaTime);
    }

    statsViewer->processInput();
}

void Game::update()
{
    debugConsole->update();
    statsViewer->update();
}

void Game::draw()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projection = glm::perspective(glm::radians(camera->fov),
        static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();

    glm::mat4 model;

    // Light
    lampShader->use();

    // Light Source
    glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
    lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
    lightPos.y = (sin(glfwGetTime()) / 2.0f) * 1.0f;
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    lampShader->setMatrix4("Model", model);
    lampShader->setMatrix4("View", view);
    lampShader->setMatrix4("Projection", projection);
    cube->render(lampShader);

    // Meshes
    meshShader->use();
    meshShader->setVector3f("LightColor", lightColor);
    meshShader->setVector3f("LightPos", lightPos);
    meshShader->setVector3f("ViewPos", camera->position);

    // Nano Suit
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.75f, -3.0f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    meshShader->setMatrix4("Model", model);
    meshShader->setMatrix4("View", view);
    meshShader->setMatrix4("Projection", projection);
    meshShader->setMatrix3("Normal", glm::mat3(glm::transpose(glm::inverse(model))));
    nanosuit->render(meshShader);

    statsViewer->render(VERSION.toLongString(), windowSize.x, windowSize.y);
    debugConsole->render(glm::vec3(1.0f));

    glfwSwapBuffers(window);
}
