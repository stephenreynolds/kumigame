#include "debugConsole.hpp"
#include "log.hpp"
#include "../input/keyboard.hpp"
#include "../util/string.hpp"
#include <fmt/format.h>
#include <string>
#include <vector>
#include <sstream>

Command DebugConsole::command;

std::string columnString(const std::string& command, const std::string& description, unsigned int width)
{
    return fmt::format("{:<{}}{:<{}}", command, width, description, width);
}

DebugConsole::DebugConsole(std::shared_ptr<TextRenderer> &textRenderer, glm::vec2 position)
    : position(position), textRenderer(textRenderer)
{
    // Toggle console visibility.
    Keyboard::addKeyBinding([this]() {
        hidden = !hidden;
    }, GLFW_KEY_GRAVE_ACCENT, GLFW_RELEASE);

    // Run input on enter.
    Keyboard::addKeyBinding([this]() {
        if (!hidden)
        {
            runCommand(input);
        }
    }, GLFW_KEY_ENTER, GLFW_PRESS);
    Keyboard::addKeyBinding([this]() {
        if (!hidden)
        {
            runCommand(input);
        }
    }, GLFW_KEY_ENTER, GLFW_REPEAT);

    // Remove character behind cursor on Backspace.
    Keyboard::addKeyBinding([this]() {
        backSpace();
    }, GLFW_KEY_BACKSPACE, GLFW_REPEAT);
    Keyboard::addKeyBinding([this]() {
        backSpace();
    }, GLFW_KEY_BACKSPACE, GLFW_PRESS);

    // Remove character in front of cursor on Delete.
    Keyboard::addKeyBinding([this]() {
        deleteChar();
    }, GLFW_KEY_DELETE, GLFW_REPEAT);
    Keyboard::addKeyBinding([this]() {
        deleteChar();
    }, GLFW_KEY_DELETE, GLFW_PRESS);

    // Move cursor on Left or Right.
    Keyboard::addKeyBinding([this]() {
        moveCursorLeft();
    }, GLFW_KEY_LEFT, GLFW_REPEAT);
    Keyboard::addKeyBinding([this]() {
        moveCursorLeft();
    }, GLFW_KEY_LEFT, GLFW_PRESS);
    Keyboard::addKeyBinding([this]() {
        moveCursorRight();
    }, GLFW_KEY_RIGHT, GLFW_REPEAT);
    Keyboard::addKeyBinding([this]() {
        moveCursorRight();
    }, GLFW_KEY_RIGHT, GLFW_PRESS);

    // Select previous input.
    // TODO: Responses should not be in command history.
    // TODO: Move cursor to end of line.
    // TODO: Allow moving both directions in history.
    Keyboard::addKeyBinding([this](){
        input = output[0];
    }, GLFW_KEY_UP, GLFW_PRESS);
    Keyboard::addKeyBinding([this](){

    }, GLFW_KEY_UP, GLFW_REPEAT);

    // Add text to input.
    for (int codePoint = 0; codePoint <= 0x007F; ++codePoint)
    {
        Keyboard::addCharBinding([this, codePoint]() {
            if (codePoint == '`') return;
            inputCharacter(codePoint);
        }, codePoint);
    }
}

void DebugConsole::update()
{
    // Print command response.
    if (!command.response.empty())
    {
        output.emplace_back(command.response);
        LOG_INFO("Debug Console: {}", command.response);
        command.response.clear();
    }

    // Run commands.
    if (!(command.processed || command.args.empty()))
    {
        if (command.args.size() == 1)
        {
            if (command[0] == "help")
            {
                const int width = 40;
                output.emplace_back(columnString("help [page number]", "Print command list.", width));
                output.emplace_back(columnString("exit|close|quit|q", "Close the console window.", width));
                output.emplace_back(columnString("exit|close|quit|q game", "Exit the game.", width));
                output.emplace_back(columnString("clear", "Clear the console.", width));
                output.emplace_back(columnString("toggle stats", "Toggle FPS, version, and other statistics.", width));
                output.emplace_back(columnString("toggle line|toggle wireframe", "Toggle wireframe polygon mode.", width));
                output.emplace_back(columnString("toggle point", "Toggle point polygon mode.", width));
                output.emplace_back(columnString("toggle fill", "Toggle fill polygon mode.", width));
                output.emplace_back(columnString("settings save", "Save settings.", width));
                output.emplace_back(columnString("set window [width:int] [height:int]", "Set the width and height of the window.", width));
                output.emplace_back(columnString("set fullscreen [bool]", "Toggle window fullscreen.", width));
                output.emplace_back(columnString("set vsync [bool]", "Turn vSync on or off.", width));
                output.emplace_back(columnString("set fov [fov:float]", "Set player's field-of-view.", width));
                output.emplace_back(columnString("Page 1/1", "", width));
                command.processed = true;
            }
            else if (command[0] == "clear")
            {
                output.clear();
                command.processed = true;
            }
            else if (command[0] == "exit" || command[0] == "close" || command[0] == "quit" || command[0] == "q")
            {
                hidden = true;
                command.processed = true;
            }
        }
    }

    if (!(command.processed || command.args.empty()))
    {
        output.emplace_back(fmt::format("No such command \"{}\".", command.input));
        command.processed = true;
    }

    // Remove oldest output when exceeding max.
    if (output.size() > MAX_OUTPUT_SIZE)
    {
        output.pop_front();
    }
}

void DebugConsole::render(glm::vec3 textColor)
{
    if (!hidden)
    {
        glDisable(GL_DEPTH_TEST); // Keep text on top.

        // Prevent text from being drawn as wireframe/points.
        int polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        auto inputPrint = fmt::format(">{}", input);

        if (cursorPosition > inputPrint.length() - 1)
        {
            inputPrint.append("|");
        }
        else
        {
            inputPrint.insert(cursorPosition + 1, "|");
        }

        for (unsigned long long i = 0; i < output.size(); ++i)
        {
            auto index = output.size() - 1 - i;
            auto outputPrint = fmt::format(">{}", output[index]);
            textRenderer->render(outputPrint, position - glm::vec2(0.0f, static_cast<float>(i + 1) * 20.0f),
                1.0f, glm::vec4(textColor, 0.5f));
        }

        textRenderer->render(inputPrint, position, 1.0f, glm::vec4(textColor, 0.7f));

        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        glEnable(GL_DEPTH_TEST); // Restore depth testing.
    }
}

void DebugConsole::backSpace()
{
    if (!(hidden || input.empty()))
    {
        if (cursorPosition > 0)
        {
            input.erase(cursorPosition - 1, 1);
            cursorPosition--;
        }
    }
}

void DebugConsole::deleteChar()
{
    if (!(hidden || input.empty()))
    {
        input.erase(cursorPosition, 1);
    }
}

void DebugConsole::moveCursorLeft()
{
    if (!hidden && cursorPosition > 0)
    {
        cursorPosition--;
    }
}

void DebugConsole::moveCursorRight()
{
    if (!hidden && cursorPosition < input.size())
    {
        cursorPosition++;
    }
}

void DebugConsole::inputCharacter(int codePoint)
{
    if (!hidden && input.size() <= MAX_INPUT_SIZE)
    {
        if (cursorPosition - 1 > input.length())
        {
            input += static_cast<char>(codePoint);
        }
        else
        {
            input.insert(cursorPosition, 1, static_cast<char>(codePoint));
        }

        cursorPosition++;
    }
}

void DebugConsole::runCommand(std::string commandString)
{
    // Save raw command input string.
    command.input = commandString;

    // Make command string lowercase and trim whitespace.
    toLower(commandString);
    trim(commandString);

    // Split command into a vector.
    command.args = split(commandString);

    output.emplace_back(commandString);
    command.processed = false;

    input.clear();
    cursorPosition = 0;
}
