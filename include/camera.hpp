#ifndef KUMIGAME_CAMERA_HPP
#define KUMIGAME_CAMERA_HPP

#include "direction.hpp"
#include <glm/glm.hpp>

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 worldUp;
    float lastX = 0.0f;
    float lastY = 0.0f;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = 45.0f;
    float sensitivity = 0.05f;
    float movementSpeed = 5.0f;

    Camera(glm::vec3 position = glm::vec3(0.0f),
           glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 getViewMatrix();

    void processKeyboard(Direction direction, float deltaTime);
    void processMouseMovement(double xPos, double yPos, bool constrainPitch = true);
    void processMouseScroll(double yOffset);

    void setMaxFOV(float maxFOV);

private:
    float maxFOV = 45.0f;

    void updateFront();
};

#endif
