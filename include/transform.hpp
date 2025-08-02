#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>


struct Transform {
    glm::vec3 pos;
    glm::quat rot;
    float scale;

    glm::mat4 to_model_mat4() const;
};

Transform interpolate(const Transform& a,
                      const Transform& b,
                      float alpha);
