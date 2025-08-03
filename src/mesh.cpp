#include "mesh.hpp"

#include <stddef.h>

#include <glad/glad.h>
#include <glm/vec3.hpp>

#include "constants.hpp"


Mesh::Mesh(const Vertex* vertices,
           const size_t  vertex_count,
           const GLuint* indices,
           const size_t  index_count) 
        : index_count(index_count)
{    
    glCreateVertexArrays(1, &vao);

    // attribute 0 - vec3 position
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, vertex_count * sizeof(Vertex), vertices, 0);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glEnableVertexArrayAttrib(vao, 0); // Activates attribute slot attribIndex for this VAO
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, x)); // Tells OpenGL how to interpret the memory at each vertex for this attribute
    glVertexArrayAttribBinding(vao, 0, 0); // Assigns the attribute to a specific binding point - “slots” where buffers (VBOs) are connected

    // element buffer
    glCreateBuffers(1, &ebo);
    glNamedBufferStorage(ebo, index_count * sizeof(GLuint), indices, 0);
    glVertexArrayElementBuffer(vao, ebo);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void Mesh::draw() const
{
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES,
                   static_cast<GLsizei>(index_count),
                   GL_UNSIGNED_INT,
                   nullptr);
}
