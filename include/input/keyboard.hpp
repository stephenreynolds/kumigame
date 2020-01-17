#ifndef KUMIGAME_INPUT_KEYBOARD_HPP
#define KUMIGAME_INPUT_KEYBOARD_HPP

#include "keyState.hpp"
#include <GLFW/glfw3.h>
#include <map>

class Keyboard
{
public:
    // @brief Sets a key's current state.
    static void set(GLFW_KEY key, GLFW_KEY_STATE keyState);
    // @brief Returns the current state of a key.
    static GLFW_KEY_STATE get(GLFW_KEY key);
    // @brief Returns whether the key is pressed.
    static bool pressed(GLFW_KEY key);
    // @brief Returns whether the key is released.
    static bool released(GLFW_KEY key);
    // @brief Returns whether the key was pressed then released.
    static bool once(GLFW_KEY key);
    // @brief Gets the keys' states from GLFW.
    static void processKeys(GLFWwindow *window);

private:
    static std::map<GLFW_KEY, KeyState> keyStates;
};

#endif
