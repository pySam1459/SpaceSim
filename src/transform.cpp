#include "transform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>


glm::mat4 Transform::to_model_mat4() const {
    return glm::translate(glm::mat4(1.0f), pos)
            * glm::mat4_cast(rot)
            * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
}

Transform interpolate(const Transform& a,
                      const Transform& b,
                      float alpha)
{
    return Transform{
        glm::mix(a.pos, b.pos, alpha),
        glm::slerp(a.rot, b.rot, alpha),
        (1.0f - alpha) * a.scale + alpha * b.scale
    };
}
