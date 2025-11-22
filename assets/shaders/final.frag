#version 450 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;      // 原始場景
uniform sampler2D bloomBlur;  // 模糊光暈
uniform float exposure;       // 曝光度 (UI 可調)

void main() {             
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    
    // 1. Additive Blending
    hdrColor += bloomColor; // 把光暈加回去

    // 2. Tone mapping (Reinhard 算法，或是更高級的 ACES)
    // vec3 result = hdrColor / (hdrColor + vec3(1.0)); // Reinhard (簡單)
    
    // 使用曝光控制的 Tone Mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    
    // 3. Gamma Correction
    result = pow(result, vec3(1.0 / gamma));
    
    FragColor = vec4(result, 1.0);
}