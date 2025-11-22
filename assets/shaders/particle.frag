#version 450 core

in vec4 vColor;
out vec4 FragColor;

void main() {
    // gl_PointCoord 是內建變數，代表像素在點內的座標 (0.0 ~ 1.0)
    // 我們將座標移到中心 (-0.5 ~ 0.5) 並算出距離
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // 1. 圓形裁切：如果距離中心超過 0.5 (半徑)，就丟棄像素
    if (dist > 0.5) {
        discard;
    }

    // 2. 柔和光暈 (Glow Falloff)
    // 讓 alpha 值隨著距離中心變小 (中心 1.0 -> 邊緣 0.0)
    // 使用 pow 讓核心更紮實，邊緣更柔和
    float alpha = 1.0 - (dist * 2.0);
    alpha = pow(alpha, 2.0); 

    // 3. 最終顏色
    // vColor.rgb 是我們在 Compute Shader 算出來的 HDR 顏色
    // 我們把它乘上 alpha，這樣邊緣會變暗且透明
    FragColor = vec4(vColor.rgb, vColor.a * alpha);
}