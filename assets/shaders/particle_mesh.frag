#version 450 core

in vec3 vNormal;
in vec3 vColor;
in vec3 vFragPos;

out vec4 FragColor;

void main() {
    // 1. 簡單的環境光
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * vec3(1.0); // 白光

    // 2. 漫反射 (Diffuse)
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3)); 
    vec3 norm = normalize(vNormal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // 3. 組合
    vec3 lighting = (ambient + diffuse) * vColor;
    
    FragColor = vec4(lighting, 1.0);
}