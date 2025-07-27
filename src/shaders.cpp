#include <glad/glad.h>

#include <stdexcept>
#include <string_view>


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