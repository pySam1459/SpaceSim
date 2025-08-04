#include "models.hpp"

#include <memory>
#include <vector>

#include <glm/trigonometric.hpp>

#include "mesh.hpp"
#include "opengl_fwd.hpp"

namespace
{
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
}

Cube::Cube(std::uint32_t idx) noexcept
        : Model(cube_vertices, std::size(cube_vertices),
                cube_indices,  std::size(cube_indices),
                idx, false) {}


namespace
{
// little help from chatgpt
void generate_uv_sphere(float radius,
                        std::uint32_t sector_count,
                        std::uint32_t stack_count,
                        std::vector<Vertex>& vertices,     // xyz nxyz uv => 8 floats each
                        std::vector<GLuint>& indices)
{
    const float pi = 3.14159265358979323846f;
    const float two_pi = 2.0f * pi;

    vertices.clear();
    indices.clear();
    vertices.reserve((stack_count + 1) * (sector_count + 1) * 8);

    for (uint32_t i = 0; i <= stack_count; ++i) {
        float v      = static_cast<float>(i) / stack_count;      // [0,1]
        float phi    = pi * v;                                   // [0,π]
        float cos_phi = std::cos(phi);
        float sin_phi = std::sin(phi);

        for (uint32_t j = 0; j <= sector_count; ++j) {
            float u      = static_cast<float>(j) / sector_count; // [0,1]
            float theta  = two_pi * u;                           // [0,2π]
            float cos_theta = std::cos(theta);
            float sin_theta = std::sin(theta);

            // Position on sphere
            float x = radius * cos_theta * sin_phi;
            float y = radius * sin_theta * sin_phi;
            float z = radius * cos_phi;

            // Normal is just the normalized position (unit sphere)
            float nx = cos_theta * sin_phi;
            float ny = sin_theta * sin_phi;
            float nz = cos_phi;

            // Append 8 floats: position (3) + normal (3) + uv (2)
            vertices.insert(vertices.end(), { x, y, z, nx, ny, nz });
        }
    }

    // Index buffer (two triangles per quad)
    for (std::uint32_t i = 0; i < stack_count; ++i) {
        std::uint32_t k1 = i * (sector_count + 1);     // start of current stack
        std::uint32_t k2 = k1 + sector_count + 1;      // start of next stack

        for (std::uint32_t j = 0; j < sector_count; ++j, ++k1, ++k2) {
            if (i != 0) {                         // skip first stack (north pole fan)
                indices.insert(indices.end(), { k1, k2, k1 + 1 });
            }
            if (i != (stack_count - 1)) {         // skip last stack (south pole fan)
                indices.insert(indices.end(), { k1 + 1, k2, k2 + 1 });
            }
        }
    }
}
}

Sphere::Sphere(std::uint32_t idx,
               bool is_light_source,
               std::vector<Vertex>& vertices,
               std::vector<GLuint>& indices) noexcept
            : Model(vertices.data(), vertices.size(),
                    indices.data(),  indices.size(),
                    idx, is_light_source) {}


std::unique_ptr<Sphere> create_sphere(std::uint32_t idx, bool is_light_source)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    generate_uv_sphere(
        1.0f, 36, 18,
        vertices, indices);

    return std::make_unique<Sphere>(idx, is_light_source, vertices, indices);
}