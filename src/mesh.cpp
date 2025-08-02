#include <mesh.hpp>

#include <glad/glad.h>

#include <stddef.h>


Mesh::Mesh(const Vertex* vertices,
           const size_t  vertex_count,
           const GLuint* indices,
           const size_t  index_count) : index_count(index_count) {
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, vertex_count * sizeof(Vertex), vertices, 0);

    glCreateVertexArrays(1, &vao);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));

    // attribute 0 - vec2 position
    glEnableVertexArrayAttrib(vao, 0); // Activates attribute slot attribIndex for this VAO
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, x)); // Tells OpenGL how to interpret the memory at each vertex for this attribute
    glVertexArrayAttribBinding(vao, 0, 0); // Assigns the attribute to a specific binding point - “slots” where buffers (VBOs) are connected

    // attribute 1 - vec3 colour
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, r));
    glVertexArrayAttribBinding(vao, 1, 0);

    glCreateBuffers(1, &ebo);
    glNamedBufferStorage(ebo, index_count * sizeof(GLuint), indices, 0);
    glVertexArrayElementBuffer(vao, ebo);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(index_count),
                   GL_UNSIGNED_INT,
                   nullptr);
}
