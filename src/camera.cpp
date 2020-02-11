#include "camera.hpp"
#include "input/keyboard.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up)
    : position(position), front(front), worldUp(up)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraDirection direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    if (direction == CameraDirection::Forward)
    {
        position += front * velocity;
    }
    if (direction == CameraDirection::Backward)
    {
        position -= front * velocity;
    }
    if (direction == CameraDirection::Left)
    {
        position -= right * velocity;
    }
    if (direction == CameraDirection::Right)
    {
        position += right * velocity;
    }
    if (direction == CameraDirection::Up)
    {
        position += up * velocity;
    }
    if (direction == CameraDirection::Down)
    {
        position -= up * velocity;
    }
}

void Camera::processMouseMovement(double xPos, double yPos, bool constrainPitch)
{
    if (firstMouse)
    {
        lastX = static_cast<float>(xPos);
        lastY = static_cast<float>(yPos);
        firstMouse = false;
    }

    float xOffset = static_cast<float>(xPos) - lastX;
    float yOffset = lastY - static_cast<float>(yPos);
    lastX = static_cast<float>(xPos);
    lastY = static_cast<float>(yPos);

    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch)
    {
        if (pitch > 89.0f)
        {
            pitch = 89.0f;
        }
        if (pitch < -89.0f)
        {
            pitch = -89.0f;
        }
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset)
{
    if (fov >= 1.0f && fov <= maxFOV)
    {
        fov -= yOffset;
    }
    if (fov < 1.0f)
    {
        fov = 1.0f;
    }
    if (fov > maxFOV)
    {
        fov = maxFOV;
    }
}

void Camera::updateCameraVectors()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}
