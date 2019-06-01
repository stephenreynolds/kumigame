#include "input/keyState.hpp"

KeyState::KeyState()
    : KeyState(-1, -1)
{
}

KeyState::KeyState(GLFW_KEY key, GLFW_KEY_STATE currentState)
    : currentState(currentState), previousState(-1)
{
}
