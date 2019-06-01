#include "input/keyboard.hpp"
#include "input/keyState.hpp"

std::map<GLFW_KEY, KeyState> Keyboard::keyStates;

void Keyboard::set(GLFW_KEY key, GLFW_KEY_STATE keyState)
{
    auto it = Keyboard::keyStates.find(key);
    KeyState state;
    if (it != Keyboard::keyStates.end())
    {
        state = it->second;
        state.previousState = state.currentState;
        state.currentState = keyState;
    }
    else
    {
        state = KeyState(key, keyState);
    }

    Keyboard::keyStates[key] = state;
}

GLFW_KEY_STATE Keyboard::get(GLFW_KEY key)
{
    return Keyboard::keyStates[key].currentState;
}

bool Keyboard::pressed(GLFW_KEY key)
{
    return Keyboard::keyStates[key].currentState == GLFW_PRESS;
}

bool Keyboard::released(GLFW_KEY key)
{
    return Keyboard::keyStates[key].currentState == GLFW_RELEASE;
}

bool Keyboard::once(GLFW_KEY key)
{
    KeyState keyState = Keyboard::keyStates[key];
    return keyState.previousState == GLFW_PRESS && keyState.currentState == GLFW_RELEASE;
}

void Keyboard::processKeys(GLFWwindow *window)
{
    for (int key = 32; key <= 348; ++key)
    {
        Keyboard::set(key, glfwGetKey(window, key));
    }
}
