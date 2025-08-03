#pragma once

#include <string_view>

#include <glm/fwd.hpp>

#include "opengl_fwd.hpp"


struct ShaderProgram {
    GLuint id{};

    GLint u_mvp{};
    GLint u_color{};

    explicit ShaderProgram(GLuint prog_id);
    ~ShaderProgram();

    void set_mvp(const glm::mat4& mvp) const;
    void set_color(const glm::vec3& color) const;
};

GLuint compile_shader(GLenum type, std::string_view src);
ShaderProgram make_program(GLuint vs, GLuint fs);
