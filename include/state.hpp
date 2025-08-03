#pragma once

#include <memory>
#include <vector>

#include "models.hpp"
#include "transform.hpp"
#include "opengl_fwd.hpp"


struct State {
    // possible future considerations,
    // combine curr and prev Transforms into single struct
    std::vector<Transform> transforms;
    std::vector<Transform> prev_tfs; // used for alpha-interpolation rendering

    std::vector<std::unique_ptr<Model>> models;

    inline void swap() { prev_tfs = transforms; }

    State() = default;
    ~State() = default;

    // no copy allowed
    State(const State&)            = delete;
    State& operator=(const State&) = delete;

    State(State&&) noexcept            = default;
    State& operator=(State&&) noexcept = default;
};