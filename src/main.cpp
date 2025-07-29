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

constexpr Vertex cube_vertices[8] = {
    { -1.0f,  1.0f,  1.0f, 1,0,0 },
    {  1.0f,  1.0f,  1.0f, 0,1,0 },
    {  1.0f, -1.0f,  1.0f, 0,0,1 },
    { -1.0f, -1.0f,  1.0f, 1,0.5f,0 },

    { -1.0f,  1.0f, -1.0f, 0.5f,0,1 },
    {  1.0f,  1.0f, -1.0f, 1,1,0 },
    {  1.0f, -1.0f, -1.0f, 0,1,0.5f },
    { -1.0f, -1.0f, -1.0f, 0.5f,0.5f,1 },
};

constexpr int kNumCube = 3 * 12;
constexpr GLuint cube_indices[kNumCube] = {
    0, 2, 1,  0, 3, 2, // front
    5, 6, 7,  5, 7, 4, // back
    4, 1, 5,  4, 0, 1, // top
    3, 6, 2,  3, 7, 6, // bottom
    4, 3, 0,  4, 7, 3, // left
    1, 6, 5,  1, 2, 6  // right
};

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
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    std::string vertexShader = read_file_to_string("shaders/basic/basic.vert"); 
    std::string fragmentShader = read_file_to_string("shaders/basic/basic.frag"); 

    GLuint prog = make_program(
        compile_shader(GL_VERTEX_SHADER, vertexShader),
        compile_shader(GL_FRAGMENT_SHADER, fragmentShader)
    );

    GLuint vbo, vao, ebo;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(cube_vertices), cube_vertices, 0);

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
    glNamedBufferStorage(ebo, sizeof(cube_indices), cube_indices, 0);
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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wire-mesh
        glUseProgram(prog);
        double now = glfwGetTime();  // time in seconds since glfwInit
        glUniform1f(glGetUniformLocation(prog, "uTime"), static_cast<float>(now));

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, kNumCube, GL_UNSIGNED_INT, nullptr);

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
