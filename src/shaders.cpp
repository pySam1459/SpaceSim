#include "shaders.hpp"

#include <stdexcept>
#include <string_view>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


ShaderProgram::ShaderProgram(GLuint prog_id) : id(prog_id)
{
    u_mvp = glGetUniformLocation(id, "u_mvp");
}

ShaderProgram::~ShaderProgram()
{
    if (id)
        glDeleteProgram(id);
}

void ShaderProgram::set_mvp(const glm::mat4& mvp) const
{
    glUniformMatrix4fv(u_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
}


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

ShaderProgram make_program(GLuint vs, GLuint fs)
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

    return ShaderProgram{prog};
}