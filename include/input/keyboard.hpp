#ifndef KUMIGAME_INPUT_KEYBOARD_HPP
#define KUMIGAME_INPUT_KEYBOARD_HPP

#include "keyState.hpp"
#include <GLFW/glfw3.h>
#include <functional>
#include <map>
#include <vector>

class Keyboard
{
public:
    static void addKeyBinding(const std::function<void()>& callback, GLFW_KEY key, GLFW_KEY_ACTION action, GLFW_KEY_MODS mods = 0);
    static void onKeyEvent(GLFW_KEY key, GLFW_KEY_ACTION action, GLFW_KEY_MODS mods);

    static void addCharBinding(const std::function<void()>& callback, unsigned int codePoint);
    static void onCharEvent(unsigned int codePoint);

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
    static std::map<GLFW_KEY, std::map<GLFW_KEY_ACTION, std::map<GLFW_KEY_MODS, std::vector<std::function<void()>>>>> keyCallbacks;
    static std::map<unsigned int, std::vector<std::function<void()>>> charCallbacks;
};

#endif
