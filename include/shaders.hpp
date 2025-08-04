#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <format>

#include <glm/fwd.hpp>

#include "opengl_fwd.hpp"

constexpr auto kNumUniforms = 6;
static const std::string uniform_names[kNumUniforms] = {"u_model", "u_vp", "u_enable_light", "u_light_pos", "u_view_pos", "u_albedo"};


struct ShaderProgram {
    GLuint id{};

    std::array<GLint, kNumUniforms> uniforms{};

    explicit ShaderProgram(GLuint prog_id);
    ~ShaderProgram();

    void set_mat4(std::string_view u_name, const glm::mat4& mat) const;
    void set_vec3(std::string_view u_name, const glm::vec3& vec) const;
    void set_bool(std::string_view u_name, const bool b) const;

private:
    inline GLint uniform_location(std::string_view name) const
    {
        for (std::size_t i=0; i<kNumUniforms; ++i) {
            if (uniform_names[i] == name)
                return uniforms[i];
        }

        throw std::runtime_error(std::format("uniform '{}' not found in cache", name));
    }
};

GLuint compile_shader(GLenum type, std::string_view src);
ShaderProgram make_program(GLuint vs, GLuint fs);
