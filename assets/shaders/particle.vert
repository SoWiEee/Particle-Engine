#version 450 core

struct Particle {
    vec4 position;
    vec4 velocity;
    vec4 color;
};

layout(std430, binding = 0) readonly buffer ParticleSSBO {
    Particle particles[];
};

out vec4 vColor;

void main() {
    Particle p = particles[gl_VertexID];

    // 把世界縮小 10 倍放入螢幕
    vec3 pos = p.position.xyz * 0.1; 
    
    gl_Position = vec4(pos, 1.0);
    
    gl_PointSize = 2.0;

    vColor = p.color;
}