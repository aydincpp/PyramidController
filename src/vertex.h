#pragma once

#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 position { 0.0f, 0.0f, 0.0f };
    glm::vec4 color { 1.0f, 1.0f, 1.0f, 1.0f };

    Vertex() = default;
    Vertex(const glm::vec3& pos, const glm::vec4& col)
        : position(pos)
        , color(col)
    {
    }
};
