#ifndef KUMIGAME_CAMERA_HPP
#define KUMIGAME_CAMERA_HPP

#include <glm/glm.hpp>

enum CameraDirection
{
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down
};

class Camera
{
public:
    const float DEFAULT_MOVEMENT_SPEED = 2.5f;
    const float DEFAULT_MOUSE_SENSITIVITY = 0.1f;
    const float DEFAULT_FOV = 45.0f;

    glm::vec3 position;
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up;
    glm::vec3 worldUp;
    glm::vec3 right;

    float lastX = 0.0f;
    float lastY = 0.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float movementSpeed = DEFAULT_MOVEMENT_SPEED;
    float mouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;
    float fov = DEFAULT_FOV;
    float maxFOV = DEFAULT_FOV;
    bool firstMouse = true;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f),
                    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 getViewMatrix();
    void processKeyboard(CameraDirection direction, float deltaTime);
    void processMouseMovement(double xPos, double yPos, bool constrainPitch = true);
    void processMouseScroll(float yOffset);

private:
    void updateCameraVectors();
};

#endif //KUMIGAME_CAMERA_HPP
