#ifndef KUMIGAME_INPUT_KEY_STATE_HPP
#define KUMIGAME_INPUT_KEY_STATE_HPP

typedef int GLFW_KEY;
typedef int GLFW_KEY_STATE;

class KeyState
{
public:
    GLFW_KEY_STATE currentState;
    GLFW_KEY_STATE previousState;

    KeyState();
    KeyState(GLFW_KEY key, GLFW_KEY_STATE currentState);
};

#endif
