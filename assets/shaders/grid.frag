#version 450 core

in vec3 WorldPos;
out vec4 FragColor;

uniform vec3 cameraPos;
uniform float farPlane;
uniform float boundarySize;

uniform vec3 attractorPos;
uniform float attractorStrength;
uniform vec3 lightColor;

float grid(vec3 worldPos, float scale) {
    vec2 coord = worldPos.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    return color.a;
}

void main() {
    // 1. 計算網格
    float g1 = grid(WorldPos, 0.1); 
    float g2 = grid(WorldPos, 1.0); 
    float gridPattern = max(g1 * 0.8, g2 * 0.3);

    // 2. 邊界發光
    float distToEdgeX = abs(WorldPos.x) - boundarySize;
    float distToEdgeZ = abs(WorldPos.z) - boundarySize;
    float edgeGlow = 0.0;
    if (abs(distToEdgeX) < 1.0 || abs(distToEdgeZ) < 1.0) {
        edgeGlow = 1.0;
    }

    // 3. 地板反光
    float distToLight = distance(WorldPos, attractorPos);
    float attenuation = 1.0 / (distToLight * distToLight * 0.01 + 1.0);
    vec3 lighting = vec3(0.0);
    if (attractorStrength != 0.0) {
        float intensity = abs(attractorStrength) * 0.02; 
        lighting = lightColor * intensity * attenuation;
    }

    // 4. 合成顏色
    vec3 floorColor = vec3(0.05, 0.05, 0.05);
    vec3 lineColor = vec3(0.6, 0.6, 0.6); 
    vec3 glowColor = vec3(1.0, 0.2, 0.1); 

    vec3 finalColor = floorColor + lineColor * gridPattern;
    finalColor = mix(finalColor, glowColor, edgeGlow * 0.5);
    finalColor += lighting * 0.5; 

    // 5. 遠處漸層消失 (Fog)
    float dist = distance(cameraPos, WorldPos);
    float alpha = 1.0 - smoothstep(farPlane * 0.1, farPlane, dist);

    FragColor = vec4(finalColor, 1.0); 
}