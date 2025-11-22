#version 450 core

struct Particle {
    vec4 position;
    vec4 velocity;
    vec4 color;
};

layout(std430, binding = 0) readonly buffer ParticleSSBO {
    Particle particles[];
};

uniform mat4 view;
uniform mat4 projection;

void main() {
    // 直接用 gl_VertexID 索引 SSBO
    Particle p = particles[gl_VertexID]; 
    
    gl_Position = projection * view * p.position;
    // ... 傳遞顏色給 fragment shader
}