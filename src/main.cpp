#include <glad/glad.h>
#include <GLFW/glfw3.h>

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


    constexpr std::string_view kVS = R"(
        #version 460 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec3 aColor;
        out vec3 vColor;
        void main() {
            vColor = aColor;
            gl_Position = vec4(aPos, 0.0, 1.0);
        })";

    constexpr std::string_view kFS = R"(
        #version 460 core
        in  vec3 vColor;
        out vec4 FragColor;
        void main() { FragColor = vec4(vColor, 1.0); })";
    
    GLuint prog = make_program(
        compile_shader(GL_VERTEX_SHADER, kVS),
        compile_shader(GL_FRAGMENT_SHADER, kFS)
    );


    GLuint vbo, vao;
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(kTriangle), kTriangle.data(), 0);

    glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    // attribute 0 - vec2 position
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, x));
    glVertexArrayAttribBinding(vao, 0, 0);

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
