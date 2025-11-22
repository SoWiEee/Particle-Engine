#pragma once
#include "../core/Buffer.hpp"
#include "../core/Shader.hpp"
#include "../core/Types.hpp"
#include <vector>
#include <memory>

class ParticleSystem {
public:
    ParticleSystem(int particleCount);
    ~ParticleSystem();

    void onUpdate(float dt, float totalTime);
    void onRender();

private:
    int m_Count;

    // 資源 (使用 smart pointers 管理生命週期)
    std::unique_ptr<Buffer> m_SSBO;
    std::unique_ptr<Shader> m_ComputeShader;
    std::unique_ptr<Shader> m_RenderShader;

    GLuint m_VAO; // Dummy VAO

    void initParticles();
};