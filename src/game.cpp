#include "game.hpp"
#include "debug/glDebug.hpp"
#include "debug/log.hpp"
#include "input/keyboard.hpp"
#include "renderer/material.hpp"
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
    LOG_INFO("Resolution: {}x{}", windowSize.x, windowSize.y);

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

    return {};
}

std::optional<std::string> Game::loadAssets()
{
    LOG_INFO("Loading assets...");
    double assetsTime = glfwGetTime();
    double time = glfwGetTime();

    // Load shaders.
    auto textShader = std::make_shared<Shader>("assets/shaders/text.vert", "assets/shaders/text.frag");
    meshShader = std::make_shared<Shader>("assets/shaders/mesh.vert", "assets/shaders/mesh.frag");
    lampShader = std::make_shared<Shader>("assets/shaders/mesh.vert", "assets/shaders/lamp.frag");
    LOG_INFO("Loaded shaders ({:.3f} ms).", 1000 * (glfwGetTime() - time));

    time = glfwGetTime();
    // Text renderer.
    textRenderer = std::make_shared<TextRenderer>(settings.width, settings.height, textShader, "assets/fonts/OCRAEXT.TTF", 14);

    // Debug console.
    debugConsole = std::make_unique<DebugConsole>(textRenderer, glm::vec2(20.0f, static_cast<float>(settings.height) - 20.0f));

    // Stats viewer.
    statsViewer = std::make_unique<StatsViewer>(textRenderer, glm::vec2(20.0f));

    // Camera
    camera = std::make_unique<Camera>();

    LOG_INFO("Loaded classes ({:.3f} ms).", 1000 * (glfwGetTime() - time));

    time = glfwGetTime();
    // Load models.
    nanosuit = std::make_unique<Model>("assets/models/nanosuit/nanosuit.obj");
    cube = std::make_unique<Model>("assets/models/cube/cube.obj");

    Texture lampTexture = {
        .id = textureFromFile("white.png", "assets/models/cube")
    };

    Material lampMaterial = {
        .diffuse = std::make_shared<Texture>(lampTexture)
    };

    lampMaterialIndex = cube->addMeshMaterial(0, lampMaterial);

    LOG_INFO("Loaded models ({:.3f} ms).", 1000 * (glfwGetTime() - time));

    LOG_INFO("Finished loading assets ({:.3f} ms).", 1000 * (glfwGetTime() - assetsTime));

    return {};
}

void Game::processInput(float deltaTime)
{
    glfwPollEvents();

    Keyboard::processKeys(window);

    // Process console commands.
    if (!(DebugConsole::command.processed || DebugConsole::command.empty()))
    {
        if (DebugConsole::command.size() == 2)
        {
            if ((DebugConsole::command[0] == "exit" || DebugConsole::command[0] == "close") && DebugConsole::command[1] == "game")
            {
                glfwSetWindowShouldClose(window, true);
                DebugConsole::command.processed = true;
                DebugConsole::command.response = "Exiting...";
            }
            else if (DebugConsole::command[0] == "settings" && DebugConsole::command[1] == "save")
            {
                saveSettings(settings, SETTINGS_PATH);
                DebugConsole::command.processed = true;
                DebugConsole::command.response = fmt::format("Settings saved at \"{}\".", SETTINGS_PATH);
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
                        DebugConsole::command.response = "Set window mode to fullscreen.";
                    }
                    else if (DebugConsole::command[2] == "false")
                    {
                        settings.fullscreen = false;
                        glfwSetWindowMonitor(window, nullptr, windowPos.x, windowPos.y, windowSize.x, windowSize.y, 0);
                        DebugConsole::command.response = "Set window mode to windowed.";
                    }

                    DebugConsole::command.processed = true;
                }
                else if (DebugConsole::command[1] == "fov")
                {
                    try
                    {
                        float fov = std::stof(DebugConsole::command[2]);
                        camera->fov = fov;
                        settings.fov = fov;
                        DebugConsole::command.response = fmt::format("Set FOV to {:.1f}.", fov);
                    }
                    catch (std::invalid_argument& ex)
                    {
                        DebugConsole::command.response = "Invalid argument: must be of type float.";
                    }

                    DebugConsole::command.processed = true;
                }
                else if (DebugConsole::command[1] == "vsync")
                {
                    if (DebugConsole::command[2] == "true")
                    {
                        glfwSwapInterval(1);
                        settings.vSync = true;
                        DebugConsole::command.response = "Vertical sync enabled.";
                    }
                    else if (DebugConsole::command[2] == "false")
                    {
                        glfwSwapInterval(0);
                        settings.vSync = false;
                        DebugConsole::command.response = "Vertical sync disabled.";
                    }

                    DebugConsole::command.processed = true;
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
                        DebugConsole::command.response = fmt::format("Set window dimensions to {0:d}x{1:d}.", width, height);
                    }
                    catch (std::invalid_argument& ex)
                    {
                        DebugConsole::command.response = "Invalid argument: must be of type int.";
                    }

                    DebugConsole::command.processed = true;
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

    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::perspective(glm::radians(camera->fov),
                                            static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 100.0f);
    glm::mat4 view = camera->getViewMatrix();

    glm::mat4 model;

    // Light
    lampShader->use();

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };

    // Light Source
    lampShader->setMatrix4("ViewProjection", projection * view);
    for (auto pos : pointLightPositions)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        lampShader->setMatrix4("Model", model);
        lampShader->setMatrix3("Normal", glm::mat3(glm::transpose(glm::inverse(model))));
        cube->render(lampShader, lampMaterialIndex);
    }

    // Meshes
    meshShader->use();

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

    // Cube
    meshShader->setMatrix4("ViewProjection", projection * view);
    meshShader->setVector3f("ViewPos", camera->position);

    meshShader->setVector3f("DirLight.direction", -0.2f, -1.0f, -0.3f);
    meshShader->setVector3f("DirLight.ambient", 0.05f, 0.05f, 0.05f);
    meshShader->setVector3f("DirLight.diffuse", 0.4f, 0.4f, 0.4f);
    meshShader->setVector3f("DirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    meshShader->setVector3f("PointLight[0].position", pointLightPositions[0]);
    meshShader->setVector3f("PointLight[0].ambient", 0.05f, 0.05f, 0.05f);
    meshShader->setVector3f("PointLight[0].diffuse", 0.8f, 0.8f, 0.8f);
    meshShader->setVector3f("PointLight[0].specular", 1.0f, 1.0f, 1.0f);
    meshShader->setFloat("PointLight[0].constant", 1.0f);
    meshShader->setFloat("PointLight[0].linear", 0.09);
    meshShader->setFloat("PointLight[0].quadratic", 0.032);
    // point light 2
    meshShader->setVector3f("PointLight[1].position", pointLightPositions[1]);
    meshShader->setVector3f("PointLight[1].ambient", 0.05f, 0.05f, 0.05f);
    meshShader->setVector3f("PointLight[1].diffuse", 0.8f, 0.8f, 0.8f);
    meshShader->setVector3f("PointLight[1].specular", 1.0f, 1.0f, 1.0f);
    meshShader->setFloat("PointLight[1].constant", 1.0f);
    meshShader->setFloat("PointLight[1].linear", 0.09);
    meshShader->setFloat("PointLight[1].quadratic", 0.032);
    // point light 3
    meshShader->setVector3f("PointLight[2].position", pointLightPositions[2]);
    meshShader->setVector3f("PointLight[2].ambient", 0.05f, 0.05f, 0.05f);
    meshShader->setVector3f("PointLight[2].diffuse", 0.8f, 0.8f, 0.8f);
    meshShader->setVector3f("PointLight[2].specular", 1.0f, 1.0f, 1.0f);
    meshShader->setFloat("PointLight[2].constant", 1.0f);
    meshShader->setFloat("PointLight[2].linear", 0.09);
    meshShader->setFloat("PointLight[2].quadratic", 0.032);
    // point light 4
    meshShader->setVector3f("PointLight[3].position", pointLightPositions[3]);
    meshShader->setVector3f("PointLight[3].ambient", 0.05f, 0.05f, 0.05f);
    meshShader->setVector3f("PointLight[3].diffuse", 0.8f, 0.8f, 0.8f);
    meshShader->setVector3f("PointLight[3].specular", 1.0f, 1.0f, 1.0f);
    meshShader->setFloat("PointLight[3].constant", 1.0f);
    meshShader->setFloat("PointLight[3].linear", 0.09);
    meshShader->setFloat("PointLight[3].quadratic", 0.032);
    // spotLight
    meshShader->setVector3f("SpotLight.position", camera->position);
    meshShader->setVector3f("SpotLight.direction", camera->front);
    meshShader->setVector3f("SpotLight.ambient", 0.0f, 0.0f, 0.0f);
    meshShader->setVector3f("SpotLight.diffuse", 1.0f, 1.0f, 1.0f);
    meshShader->setVector3f("SpotLight.specular", 1.0f, 1.0f, 1.0f);
    meshShader->setFloat("SpotLight.constant", 1.0f);
    meshShader->setFloat("SpotLight.linear", 0.09);
    meshShader->setFloat("SpotLight.quadratic", 0.032);
    meshShader->setFloat("SpotLight.cutOff", glm::cos(glm::radians(12.5f)));
    meshShader->setFloat("SpotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    for (unsigned int i = 0; i < 10; ++i)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        meshShader->setMatrix4("Model", model);
        meshShader->setMatrix3("Normal", glm::mat3(glm::transpose(glm::inverse(model))));
        cube->render(meshShader);
    }

    // Nano Suit
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, -1.75f, -3.0f));
//    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
//    meshShader->setMatrix4("Model", model);
//    meshShader->setMatrix4("View", view);
//    meshShader->setMatrix4("Projection", projection);
//    meshShader->setMatrix3("Normal", glm::mat3(glm::transpose(glm::inverse(model))));
//    meshShader->setVector3f("ViewPos", camera->position);
//    nanosuit->render(meshShader);

    statsViewer->render(VERSION.toLongString(), windowSize.x, windowSize.y);
    debugConsole->render(glm::vec3(1.0f));

    glfwSwapBuffers(window);
}
