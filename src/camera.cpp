#include <camera.hpp>
#include <constants.hpp>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


constexpr float kCameraMul = 2.5f;
constexpr float kMouseSensitivity = 0.1f;


Camera::Camera(glm::vec3 pos, glm::vec3 up) :
    position(pos), front({0, 0, -1}), up(up),
    mls{-90.0f, 0.0f, kWidth/2.0f, kHeight/2.0f}
{}

glm::mat4 Camera::getView() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::keyInput(GLFWwindow* window, float deltaTime)
{
    const float speed = kCameraMul * deltaTime;

    glm::vec3 right = glm::normalize(glm::cross(front, up));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += up * speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= up * speed;
}

void Camera::window_setup(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, Camera::mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

static bool firstMouse = true;

void Camera::on_mouse_move(double xpos, double ypos)
{
    if (firstMouse) {
        mls.lastX = float(xpos);
        mls.lastY = float(ypos);
        firstMouse = false;
    }

    float xoffset = float(xpos) - mls.lastX;
    float yoffset = mls.lastY - float(ypos);
    mls.lastX = float(xpos);
    mls.lastY = float(ypos);

    mls.yaw   += xoffset * kMouseSensitivity;
    mls.pitch += yoffset * kMouseSensitivity;

    // Clamp pitch to avoid flip
    if (mls.pitch > 89.0f)  mls.pitch = 89.0f;
    if (mls.pitch < -89.0f) mls.pitch = -89.0f;

    front = glm::normalize(glm::vec3{
        cos(glm::radians(mls.yaw)) * cos(glm::radians(mls.pitch)),
        sin(glm::radians(mls.pitch)),
        sin(glm::radians(mls.yaw)) * cos(glm::radians(mls.pitch))
    });
}