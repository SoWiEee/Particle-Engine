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
    float pointScale = 2500.0f;
    float respawnHeight = -20.0f;
    glm::vec3 attractorPos = glm::vec3(0.0f);
	float attractorStrength = 0.0f; // disabled by default
    float bounce = 0.5f;
    int particleCount = 100000;
    float boundarySize = 80.0f;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class ParticleSystem {
public:
    ParticleSystem(int particleCount);
    ~ParticleSystem();
    ParticleProps Props;

    void onUpdate(float dt, float totalTime);
    void onRender(const Camera& camera);
    void reset() { initParticles(); }

private:
    int m_Count;
    std::unique_ptr<Buffer> m_SSBO;
    std::unique_ptr<Shader> m_ComputeShader;
    std::unique_ptr<Shader> m_RenderShader;
    GLuint m_VAO; // Dummy VAO
    void initParticles();

    GLuint m_MeshVAO;
    GLuint m_MeshVBO;
    GLuint m_MeshEBO;
    int m_IndexCount; // 紀錄要畫多少個索引
    void initCubeMesh();
};