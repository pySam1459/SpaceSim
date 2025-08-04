#include "shaders.hpp"

#include <iostream>
#include <stdexcept>
#include <string_view>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


ShaderProgram::ShaderProgram(GLuint prog_id) : id(prog_id)
{
    for (std::size_t i=0; i<kNumUniforms; ++i) {
        uniforms[i] = glGetUniformLocation(id, uniform_names[i].data());
        if (uniforms[i] == -1)
            std::cerr << "Warning: No uniform found with name `" << uniform_names[i] << "`\n";
    }
}

ShaderProgram::~ShaderProgram()
{
    if (id)
        glDeleteProgram(id);
}

void ShaderProgram::set_mat4(std::string_view u_name, const glm::mat4& mat) const
{
    GLint handle = uniform_location(u_name);
    glUniformMatrix4fv(handle, 1, GL_FALSE, glm::value_ptr(mat));
}
void ShaderProgram::set_vec3(std::string_view u_name, const glm::vec3& vec) const
{
    GLint handle = uniform_location(u_name);
    glUniform3fv(handle, 1, glm::value_ptr(vec));
}
void ShaderProgram::set_bool(std::string_view u_name, const bool b) const
{
    GLint handle = uniform_location(u_name);
    glUniform1i(handle, b);
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