#include "ParticleSystem.hpp"
#include <random>
#include <iostream>

ParticleSystem::ParticleSystem(int particleCount) : m_Count(particleCount) {
    m_ComputeShader = std::make_unique<Shader>("assets/shaders/particle.comp");
    m_RenderShader = std::make_unique<Shader>("assets/shaders/particle.vert", "assets/shaders/particle.frag");

    initParticles();

    // empty VAO
    glCreateVertexArrays(1, &m_VAO);
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &m_VAO);
}

void ParticleSystem::initParticles() {
    std::vector<Particle> data(m_Count);

    // rng
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distPos(-10.0f, 10.0f);
    std::uniform_real_distribution<float> distVel(-2.0f, 2.0f);

    for (auto& p : data) {
        p.position = glm::vec4(distPos(gen), distPos(gen) + 10.0f, distPos(gen), 1.0f);
        p.velocity = glm::vec4(distVel(gen), distVel(gen), distVel(gen), 0.0f);
        p.color = glm::vec4(1.0f); // 先給全白測試
    }

    // create SSBO
    m_SSBO = std::make_unique<Buffer>(
        sizeof(Particle) * m_Count,
        data.data(),
        GL_DYNAMIC_STORAGE_BIT
    );
}

void ParticleSystem::onUpdate(float dt, float totalTime) {
    m_ComputeShader->use();
    m_ComputeShader->setFloat("dt", dt);
    m_ComputeShader->setFloat("time", totalTime);
    m_ComputeShader->setInt("pCount", m_Count);

    // 傳遞參數
    m_ComputeShader->setVec3("gravity", Props.gravity);
    m_ComputeShader->setFloat("speed", Props.emitSpeed);
    m_ComputeShader->setFloat("lifeTime", Props.respawnHeight);

    // 傳遞互動參數
    m_ComputeShader->setVec3("attractorPos", Props.attractorPos);
    m_ComputeShader->setFloat("attractorStrength", Props.attractorStrength);
    m_ComputeShader->setFloat("bounce", Props.bounce);
    m_ComputeShader->setFloat("boundarySize", Props.boundarySize);

    m_SSBO->bindBase(0);
    glDispatchCompute((m_Count + 127) / 128, 1, 1);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::onRender(const Camera& camera) {
    m_RenderShader->use();
    m_RenderShader->setMat4("view", camera.GetViewMatrix());
    m_RenderShader->setMat4("projection", camera.GetProjectionMatrix(1280.0f, 720.0f));
    m_RenderShader->setFloat("pointScale", Props.pointScale);

    m_SSBO->bindBase(0);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_POINTS, 0, m_Count);
    glBindVertexArray(0);
}