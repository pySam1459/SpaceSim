#pragma once

#include <memory>
#include <iterator>
#include <vector>

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


struct Cube : Model {
    Cube(std::uint32_t idx) noexcept;
};

struct Sphere : Model {
    Sphere(std::uint32_t idx,
           std::vector<Vertex>& vertices,
           std::vector<GLuint>& indices) noexcept;

    Sphere(const Sphere&) = delete;
    Sphere& operator=(const Sphere&) = delete;

    Sphere(Sphere&&) noexcept = default;
    Sphere& operator=(Sphere&&) noexcept = default;
};

std::unique_ptr<Sphere> create_sphere(std::uint32_t idx);
