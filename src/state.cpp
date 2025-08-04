#include "state.hpp"

#include <glm/glm.hpp>

#include "constants.hpp"

constexpr float G = 10.0f;


void State::tick(float dt)
{
    for (std::size_t i=0; i<kNumObjects; ++i) {
        PhysicsProps& p = props[i];
        Transform& tf = transforms[i];

        glm::vec3 force{0};
        for (std::size_t j=0; j<kNumObjects; ++j) {
            if (i == j) continue;
            
            glm::vec3 r_vec = transforms[j].pos - tf.pos;
            float r_sq = dot(r_vec, r_vec);
            float r = sqrt(r_sq);

            float force_mag = G * p.mass * props[j].mass / r_sq;
            force += force_mag * r_vec / r;
        }
 
        p.vel += (force / p.mass) * dt; // apply acceleration
        tf.pos += p.vel * dt;
    }
}