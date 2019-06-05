#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
    : position(position), front(front), worldUp(up)
{
    updateFront();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(Direction direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    if (direction == Direction::Forward)
        position += velocity * front;
    if (direction == Direction::Backward)
        position -= velocity * front;
    if (direction == Direction::Left)
        position -= glm::normalize(glm::cross(front, up)) * velocity;
    if (direction == Direction::Right)
        position += glm::normalize(glm::cross(front, up)) * velocity;
}

void Camera::processMouseMovement(double xPos, double yPos, bool constrainPitch)
{
    float xOffset = static_cast<float>(xPos) - lastX;
    float yOffset = lastY - static_cast<float>(yPos);
    lastX = static_cast<float>(xPos);
    lastY = static_cast<float>(yPos);

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateFront();
}

void Camera::processMouseScroll(double yOffset)
{
    if (fov >= 1.0f && fov <= maxFOV)
        fov -= static_cast<float>(yOffset);
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= maxFOV)
        fov = maxFOV;
}

void Camera::updateFront()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::setMaxFOV(float maxFOV)
{
    this->maxFOV = maxFOV;
    fov = maxFOV;
}
