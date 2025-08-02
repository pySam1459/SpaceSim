#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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
    static void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
        auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        if (cam)
            cam->on_mouse_move(xpos, ypos);
    }
    void on_mouse_move(double xpos, double ypos);
};
