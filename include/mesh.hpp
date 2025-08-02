#pragma once
#include <glad/glad.h>

struct Vertex {
    float x, y, z;
    float r, g, b;
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

    void draw();
};
