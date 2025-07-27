#include <glad/glad.h>

#include <string_view>


GLuint compile_shader(GLenum type, std::string_view src);

GLuint make_program(GLuint vs, GLuint fs);