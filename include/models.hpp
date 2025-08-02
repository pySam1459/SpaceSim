#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <mesh.hpp>

#include <iterator>


struct Model {
    glm::vec3 pos;
    glm::vec3 rot;
    float scale;

    Mesh mesh;

    Model(const Vertex* vertices,
          size_t        vertex_count,
          const GLuint* indices,
          size_t        index_count,
          glm::vec3     pos, 
          glm::vec3     rot   = {0,0,0},
          float         scale = 1.0f)
        : pos  (pos)
        , rot  (rot)
        , scale(scale)
        , mesh (vertices, vertex_count, indices, index_count)
    {}

    glm::mat4 get_model_mat4() const {
        glm::quat q = glm::quat(glm::radians(rot));
        return glm::translate(glm::mat4(1.0f), pos)
             * glm::toMat4(q)
             * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
    }

    void draw() {
        mesh.draw();
    }
};


constexpr Vertex cube_vertices[8] = {
    { -1.0f,  1.0f,  1.0f, 1,0,0 },
    {  1.0f,  1.0f,  1.0f, 0,1,0 },
    {  1.0f, -1.0f,  1.0f, 0,0,1 },
    { -1.0f, -1.0f,  1.0f, 1,0.5f,0 },

    { -1.0f,  1.0f, -1.0f, 0.5f,0,1 },
    {  1.0f,  1.0f, -1.0f, 1,1,0 },
    {  1.0f, -1.0f, -1.0f, 0,1,0.5f },
    { -1.0f, -1.0f, -1.0f, 0.5f,0.5f,1 },
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
    Cube(glm::vec3 pos, glm::vec3 rot, float scale)
        : Model(cube_vertices, std::size(cube_vertices),
                cube_indices, std::size(cube_indices),
                pos, rot, scale) {}
};