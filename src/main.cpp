#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <constants.hpp>
#include <shaders.hpp>
#include <camera.hpp>
#include <read_file_to_string.hpp>

#include <iostream>
#include <stdexcept>
#include <string_view>


struct Vertex {
    float x, y, z;
    float r, g, b;
};

constexpr Vertex vertices[4] = {
    { -0.5f, -0.5f, 0.0f, 1,0,0 },
    {  0.5f, -0.5f, 0.0f, 0,1,0 },
    {  0.5f,  0.5f, 0.0f, 0,0,1 },
    { -0.5f,  0.5f, 0.0f, 1,1,0 }
};
constexpr GLuint indices[6] = { 0, 1, 2,  2, 3, 0 };

void framebuffer_size_callback(GLFWwindow*, int w, int h)
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

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Space Sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        glfwTerminate();
        throw std::runtime_error("gladLoadGL failed");
    }

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1); // v-sync

    std::string vertexShader = read_file_to_string("shaders/basic/triangle.vert"); 
    std::string fragmentShader = read_file_to_string("shaders/basic/triangle.frag"); 

    GLuint prog = make_program(
        compile_shader(GL_VERTEX_SHADER, vertexShader),
        compile_shader(GL_FRAGMENT_SHADER, fragmentShader)
    );

    GLuint vbo, vao, ebo;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(vertices), vertices, 0);

    glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    // attribute 0 - vec2 position
    glEnableVertexArrayAttrib(vao, 0); // Activates attribute slot attribIndex for this VAO
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, x)); // Tells OpenGL how to interpret the memory at each vertex for this attribute
    glVertexArrayAttribBinding(vao, 0, 0); // Assigns the attribute to a specific binding point - “slots” where buffers (VBOs) are connected

    // attribute 1 - vec3 colour
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, r));
    glVertexArrayAttribBinding(vao, 1, 0);

    glCreateBuffers(1, &ebo);
    glNamedBufferStorage(ebo, sizeof(indices), indices, 0);
    glVertexArrayElementBuffer(vao, ebo);

    Camera cam({0.0f, 0.0f, 3.0f}, {0.0f, 1.0f, 0.0f});
    glfwSetWindowUserPointer(window, &cam);
    glfwSetCursorPosCallback(window, Camera::mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    bool running = true;
    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window) && running) {
        float currentTime = glfwGetTime();
        float dt = currentTime - lastTime;
        lastTime = currentTime;

        cam.keyInput(window, dt);
        glm::mat4 view = cam.getView();
        glm::mat4 proj = glm::perspective(glm::radians(60.0f), float(kWidth)/kHeight, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = proj * view * model;
        glUniformMatrix4fv(glGetUniformLocation(prog, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));


        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        double now = glfwGetTime();  // time in seconds since glfwInit
        glUniform1f(glGetUniformLocation(prog, "uTime"), static_cast<float>(now));

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            running = false;
    }

    glDeleteProgram(prog);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

} catch (std::exception& e) {
    std::cerr << "[FATAL] " << e.what() << std::endl;
    return EXIT_FAILURE;
}
