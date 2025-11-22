#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

struct Particle {
    vec4 position;
    vec4 velocity;
    vec4 color;
};
layout(std430, binding = 0) readonly buffer ParticleSSBO {
    Particle particles[];
};

// Uniforms
uniform mat4 view;
uniform mat4 projection;
uniform float pointScale; // 石頭的大小倍率

out vec3 vNormal;
out vec3 vColor;
out vec3 vFragPos;

void main() {
    // 1. 取得當前粒子數據
    Particle p = particles[gl_InstanceID];

    // 2. 計算模型矩陣 (Model Matrix)
    // 我們先做最簡單的：位移 + 縮放
    
    // 基礎大小：假設 pointScale 是 2000，我們先縮小一點讓石頭合理
    float size = pointScale * 0.0002; 
    
    // 根據速度稍微拉長石頭 (Stretch)，增加速度感
    vec3 scale = vec3(size);
    
    mat4 model = mat4(1.0);
    model[0][0] = scale.x;
    model[1][1] = scale.y;
    model[2][2] = scale.z;
    model[3] = vec4(p.position.xyz, 1.0);

    // 3. 計算最終位置
    vec4 worldPos = model * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    
    gl_Position = projection * view * worldPos;

    // 4. 傳遞法線與顏色
    vNormal = aNormal;
    vColor = p.color.rgb;
}