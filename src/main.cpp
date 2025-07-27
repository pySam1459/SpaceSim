#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <read_file_to_string.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <string_view>


constexpr int kWidth = 1280;
constexpr int kHeight = 720;

GLuint compile_shader(GLenum type, std::string_view src)
{
    GLuint id = glCreateShader(type);
    const char* csrc = src.data();
    glShaderSource(id, 1, &csrc, nullptr);
    glCompileShader(id);

    GLint ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(id, sizeof log, nullptr, log);
        throw std::runtime_error(std::string("shader compile error: ") + log);
    }
    return id;
}

GLuint make_program(GLuint vs, GLuint fs)
{
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(prog, sizeof log, nullptr, log);
        throw std::runtime_error(std::string("link error: ") + log);
    }

    glDetachShader(prog, vs);
    glDetachShader(prog, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

struct Vertex {
    float x, y;
    float r, g, b;
};

constexpr std::array<Vertex, 3> kTriangle = {{
    {  0.0f,  0.6f, 1.0f, 0.0f, 0.0f },
    { -0.7f, -0.6f, 0.0f, 1.0f, 0.0f },
    {  0.7f, -0.6f, 0.0f, 0.0f, 1.0f }
}};

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

    // std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n" << "OpenGL version " << glGetString(GL_VERSION) << std::endl;

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


    GLuint vbo, vao;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(kTriangle), kTriangle.data(), 0);

    glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    // attribute 0 - vec2 position
    glEnableVertexArrayAttrib(vao, 0); // Activates attribute slot attribIndex for this VAO
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, x)); // Tells OpenGL how to interpret the memory at each vertex for this attribute
    glVertexArrayAttribBinding(vao, 0, 0); // Assigns the attribute to a specific binding point - “slots” where buffers (VBOs) are connected

    // attribute 1 - vec3 colour
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, r));
    glVertexArrayAttribBinding(vao, 1, 0);


    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
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
