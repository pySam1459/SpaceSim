#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

constexpr int FRAME_WIDTH = 1280;
constexpr int FRAME_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* /*window*/, int w, int h)
{
    glViewport(0, 0, w, h);
}


int main()
try {
    if (!glfwInit())
        throw std::runtime_error("GLFW initilisation failed");
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(FRAME_WIDTH, FRAME_HEIGHT, "Space Sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        glfwTerminate();
        throw std::runtime_error("gladLoadGL failed");
    }

    std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n" << "OpenGL version " << glGetString(GL_VERSION) << std::endl;

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.39f, 0.58f, 0.93f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

} catch (std::exception& e) {
    std::cerr << "[FATAL] " << e.what() << std::endl;
    return EXIT_FAILURE;
}
