#ifndef KUMIGAME_DEBUG_DEBUG_CONSOLE_HPP
#define KUMIGAME_DEBUG_DEBUG_CONSOLE_HPP

#include "renderer/textRenderer.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <deque>
#include <memory>
#include <string>
#include <vector>

class DebugConsole
{
public:
    static std::vector<std::string> command;
    static bool commandProcessed;
    static std::string response;

    glm::vec2 position;
    bool hidden = true;

    DebugConsole(std::shared_ptr<TextRenderer> &textRenderer, glm::vec2 position);

    void update();
    void render(glm::vec3 textColor = glm::vec3(0.0f));

private:
    const unsigned short MAX_INPUT_SIZE = 255;
    const unsigned short MAX_OUTPUT_SIZE = 25;
    std::shared_ptr<TextRenderer> textRenderer;
    std::string input;
    std::deque<std::string> output;
    size_t cursorPosition = 0;

    void runCommand(std::string commandString);

    void backSpace();
    void deleteChar();
    void moveCursorRight();
    void moveCursorLeft();
    void inputCharacter(int codePoint);
};

#endif //KUMIGAME_DEBUG_DEBUG_CONSOLE_HPP
