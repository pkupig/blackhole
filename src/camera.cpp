#include "camera.h"
#include <GLFW/glfw3.h>
#include <algorithm>

Camera::Camera(glm::vec3 target, float radius, glm::vec3 worldUp)
    : Target(target), Radius(radius), WorldUp(worldUp) {

    MinRadius = 2.0f;
    MaxRadius = 100.0f;
    MovementSpeed = 10.0f;
    MouseSensitivity = 0.1f;
    ZoomSensitivity = 2.0f;
    Zoom = 45.0f;

    Theta = 45.0f;  
    Phi = 30.0f;  

    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Target, Up);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Theta += xoffset;
    Phi += yoffset;

    if (constrainPitch) {
        if (Phi > 89.0f)
            Phi = 89.0f;
        if (Phi < -89.0f)
            Phi = -89.0f;
    }

    if (Theta > 360.0f) Theta -= 360.0f;
    if (Theta < 0.0f) Theta += 360.0f;

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    Radius -= yoffset * ZoomSensitivity;
    Radius = glm::clamp(Radius, MinRadius, MaxRadius);
    updateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;

    glm::vec3 forward = glm::normalize(Target - Position);
    glm::vec3 right = glm::normalize(glm::cross(forward, WorldUp));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    switch (direction) {
    case FORWARD:
        Target += forward * velocity;
        break;
    case BACKWARD:
        Target -= forward * velocity;
        break;
    case LEFT:
        Target -= right * velocity;
        break;
    case RIGHT:
        Target += right * velocity;
        break;
    case UP:
        Target += up * velocity;
        break;
    case DOWN:
        Target -= up * velocity;
        break;
    }

    updateCameraVectors();
}

void Camera::SetTarget(glm::vec3 newTarget) {
    Target = newTarget;
    updateCameraVectors();
}

void Camera::SetRadius(float newRadius) {
    Radius = glm::clamp(newRadius, MinRadius, MaxRadius);
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    float phiRad = glm::radians(Phi);
    float thetaRad = glm::radians(Theta);

    Position.x = Target.x + Radius * cos(phiRad) * sin(thetaRad);
    Position.y = Target.y + Radius * sin(phiRad);
    Position.z = Target.z + Radius * cos(phiRad) * cos(thetaRad);

    Front = glm::normalize(Target - Position);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}