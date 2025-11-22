#pragma once
#include "../core/Buffer.hpp"
#include "../core/Shader.hpp"
#include "../core/Types.hpp"
#include "../core/Camera.hpp"
#include <vector>
#include <memory>

struct ParticleProps {
    glm::vec3 gravity = glm::vec3(0.0f, -9.8f, 0.0f);
    float emitSpeed = 15.0f;
    float pointScale = 500.0f; // 點的基礎大小
    float respawnHeight = -10.0f;
    int particleCount = 10000; // 唯讀，目前不支援動態改大小
};

class ParticleSystem {
public:
    ParticleSystem(int particleCount);
    ~ParticleSystem();
    ParticleProps Props;

    void onUpdate(float dt, float totalTime);
    void onRender(const Camera& camera);

private:
    int m_Count;

    // 資源 (使用 smart pointers 管理生命週期)
    std::unique_ptr<Buffer> m_SSBO;
    std::unique_ptr<Shader> m_ComputeShader;
    std::unique_ptr<Shader> m_RenderShader;

    GLuint m_VAO; // Dummy VAO

    void initParticles();
};