#pragma once

struct Particle {
    glm::vec4 position; // xyz = pos, w = life
    glm::vec4 velocity; // xyz = vel, w = size/mass
    glm::vec4 color;    // rgba
    // 總共 3 * 16 = 48 bytes。
    // 顯卡喜歡 16 bytes 對齊，這結構非常完美。
};