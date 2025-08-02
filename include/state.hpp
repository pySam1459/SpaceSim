#pragma once

#include <memory>
#include <vector>

#include "models.hpp"
#include "transform.hpp"


struct State {
    // possible future considerations,
    // combine curr and prev Transforms into single struct
    std::vector<Transform> transforms;
    std::vector<Transform> prev_tfs; // used for alpha-interpolation rendering

    std::vector<std::unique_ptr<Model>> models;

    inline void swap() { prev_tfs = transforms; }
};