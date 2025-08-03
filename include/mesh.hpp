#pragma once

#include "opengl_fwd.hpp"


struct Vertex {
    float x, y, z;
};

struct Mesh {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    size_t index_count;

    Mesh(const Vertex* vertices,
         const size_t  vertex_count,
         const GLuint* indices,
         const size_t  index_count);
    ~Mesh();

    void draw() const;
};
