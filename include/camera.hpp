#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "opengl_fwd.hpp"

struct GLFWwindow;

struct MouseLookState {
    float yaw, pitch;
    float lastX, lastY;
};

struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;

    MouseLookState mls;

    Camera(glm::vec3 pos, glm::vec3 up);

    glm::mat4 getView() const;
    void keyInput(GLFWwindow* window, float deltaTime);

    void window_setup(GLFWwindow* window);
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
    void on_mouse_move(double xpos, double ypos);
};
