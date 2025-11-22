#version 450 core
struct Particle { vec4 position; vec4 velocity; vec4 color; };
layout(std430, binding = 0) readonly buffer ParticleSSBO { Particle particles[]; };

uniform mat4 view;
uniform mat4 projection;
uniform float pointScale;

out vec4 vColor;

void main() {
    Particle p = particles[gl_VertexID];
    
    vec4 worldPos = vec4(p.position.xyz, 1.0);
    
    gl_Position = projection * view * worldPos;
    
    // 根據距離調整點大小
    float dist = length(gl_Position.xyz);
    gl_PointSize = pointScale / dist;

    vColor = p.color;
}