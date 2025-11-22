#include "ParticleSystem.hpp"
#include <random>

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

    for (auto& p : data) {
        p.position = glm::vec4(0.0f);
        p.velocity = glm::vec4(0.0f);
        p.color = glm::vec4(1.0f);
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

    // SSBO -> binding point 0
    m_SSBO->bindBase(0);

    // Dispatch
    int numGroups = (m_Count + 127) / 128;
    glDispatchCompute(numGroups, 1, 1);

    // Barrier
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::onRender() {
    m_RenderShader->use();

    // bind SSBO
    m_SSBO->bindBase(0);

    glBindVertexArray(m_VAO);

    // draw points
    glDrawArrays(GL_POINTS, 0, m_Count);

    glBindVertexArray(0);
}