#pragma once
#include <glm/glm.hpp>

struct alignas(16) Particle {
    glm::vec4 position; // xyz = pos, w = life
    glm::vec4 velocity; // xyz = vel, w = padding/mass
    glm::vec4 color;    // rgba
};