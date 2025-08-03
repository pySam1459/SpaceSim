#pragma once

#include <iterator>

#include "mesh.hpp"
#include "opengl_fwd.hpp"


struct Model {
    std::uint32_t idx;
    Mesh mesh;

    Model(const Vertex* vertices,
          size_t        vertex_count,
          const GLuint* indices,
          size_t        index_count,
          const std::uint32_t idx)
        : idx(idx),
          mesh (vertices, vertex_count, indices, index_count)
    {}

    void draw() const {
        mesh.draw();
    }
};


constexpr Vertex cube_vertices[8] = {
    { -1.0f,  1.0f,  1.0f },
    {  1.0f,  1.0f,  1.0f },
    {  1.0f, -1.0f,  1.0f },
    { -1.0f, -1.0f,  1.0f },

    { -1.0f,  1.0f, -1.0f },
    {  1.0f,  1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },
    { -1.0f, -1.0f, -1.0f },
};

constexpr GLuint cube_indices[36] = {
    0, 2, 1,  0, 3, 2, // front
    5, 6, 7,  5, 7, 4, // back
    4, 1, 5,  4, 0, 1, // top
    3, 6, 2,  3, 7, 6, // bottom
    4, 3, 0,  4, 7, 3, // left
    1, 6, 5,  1, 2, 6  // right
};

struct Cube : public Model {
    Cube(std::uint32_t idx) noexcept
        : Model(cube_vertices, std::size(cube_vertices),
                cube_indices, std::size(cube_indices),
                idx) {}
};