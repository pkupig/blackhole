#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Target;
    glm::vec3 WorldUp;

    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up;

    float Radius;
    float MinRadius;
    float MaxRadius;

    float Theta;    // 水平角 (0-360度)
    float Phi;      // 垂直角 (-89到89度)

    float MovementSpeed;
    float MouseSensitivity;
    float ZoomSensitivity;
    float Zoom;

    Camera(glm::vec3 target = glm::vec3(0.0f),
        float radius = 25.0f,
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 GetViewMatrix();

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void SetTarget(glm::vec3 newTarget);
    void SetRadius(float newRadius);

private:
    void updateCameraVectors();
};

#endif