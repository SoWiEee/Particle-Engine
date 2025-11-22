#version 450 core

in vec3 WorldPos;
out vec4 FragColor;

uniform vec3 cameraPos;
uniform float farPlane;
uniform float boundarySize;

// 網格生成函數
float grid(vec3 worldPos, float scale) {
    vec2 coord = worldPos.xz * scale; // 只看 XZ 平面
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    return color.a;
}

void main() {
    // 1. 計算網格 (兩個層次：大格子和小格子)
    float g1 = grid(WorldPos, 0.1); // 大格子 (每 10 單位)
    float g2 = grid(WorldPos, 1.0); // 小格子 (每 1 單位)
    
    // 混合大小格子，讓大格子比較亮
    float gridPattern = max(g1 * 0.8, g2 * 0.3);

    // 2. 邊界發光 (Boundary Glow)
    // 讓我們知道牆壁在哪裡
    float distToEdgeX = abs(WorldPos.x) - boundarySize;
    float distToEdgeZ = abs(WorldPos.z) - boundarySize;
    float edgeGlow = 0.0;
    
    // 如果靠近邊界 (例如 2 單位內)，就發光
    if (abs(distToEdgeX) < 1.0 || abs(distToEdgeZ) < 1.0) {
        edgeGlow = 1.0;
    }

    // 3. 基礎顏色 (深灰色地板)
    vec3 floorColor = vec3(0.05, 0.05, 0.05);
    vec3 lineColor = vec3(0.6, 0.6, 0.6); // 線條顏色
    vec3 glowColor = vec3(1.0, 0.2, 0.1); // 邊界警告色 (紅橘色)

    vec3 finalColor = floorColor + lineColor * gridPattern;
    finalColor = mix(finalColor, glowColor, edgeGlow * 0.5);

    // 4. 距離漸層 (Fog)
    // 讓遠處的地板慢慢變黑，融合進背景
    float dist = distance(cameraPos, WorldPos);
    float alpha = 1.0 - smoothstep(farPlane * 0.1, farPlane, dist);

    FragColor = vec4(finalColor, 1.0);
}