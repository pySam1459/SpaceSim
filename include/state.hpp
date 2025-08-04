#pragma once

#include <memory>
#include <vector>

#include <glm/vec3.hpp>

#include "models.hpp"
#include "transform.hpp"
#include "opengl_fwd.hpp"


struct PhysicsProps {
    glm::vec3 vel;
    float mass;
};


struct State {
    // possible future considerations,
    // combine curr and prev Transforms into single struct
    std::vector<Transform> transforms;
    std::vector<Transform> prev_tfs; // used for alpha-interpolation rendering

    std::vector<PhysicsProps> props;
    std::vector<std::unique_ptr<Model>> models;

    inline void swap() { prev_tfs = transforms; }

    State() = default;
    ~State() = default;

    void tick(float dt);

    // no copy allowed
    State(const State&)            = delete;
    State& operator=(const State&) = delete;

    State(State&&) noexcept            = default;
    State& operator=(State&&) noexcept = default;
};