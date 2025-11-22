#include "ParticleSystem.hpp"
#include <random>
#include <iostream>

ParticleSystem::ParticleSystem(int particleCount) : m_Count(particleCount) {
    m_ComputeShader = std::make_unique<Shader>("assets/shaders/particle.comp");
    m_RenderShader = std::make_unique<Shader>("assets/shaders/particle.vert", "assets/shaders/particle.frag");
    m_RenderShader = std::make_unique<Shader>("assets/shaders/particle_mesh.vert", "assets/shaders/particle_mesh.frag");

    initParticles();
    initCubeMesh();

    // empty VAO
    glCreateVertexArrays(1, &m_VAO);
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &m_MeshVAO);
    glDeleteBuffers(1, &m_MeshVBO);
    glDeleteBuffers(1, &m_MeshEBO);
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
    glBindVertexArray(m_MeshVAO);
    glDrawElementsInstanced(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0, m_Count);
    glBindVertexArray(0);
}

void ParticleSystem::initCubeMesh() {
    // 1. 立方體的頂點
    std::vector<Vertex> vertices = {
        // Position (XYZ)        // Normal (XYZ) - 簡化版指向外側
        {{-0.5f, -0.5f,  0.5f},  {-0.5f, -0.5f,  0.5f}}, // 左下前
        {{ 0.5f, -0.5f,  0.5f},  { 0.5f, -0.5f,  0.5f}}, // 右下前
        {{ 0.5f,  0.5f,  0.5f},  { 0.5f,  0.5f,  0.5f}}, // 右上前
        {{-0.5f,  0.5f,  0.5f},  {-0.5f,  0.5f,  0.5f}}, // 左上前
        {{-0.5f, -0.5f, -0.5f},  {-0.5f, -0.5f, -0.5f}}, // 左下後
        {{ 0.5f, -0.5f, -0.5f},  { 0.5f, -0.5f, -0.5f}}, // 右下後
        {{ 0.5f,  0.5f, -0.5f},  { 0.5f,  0.5f, -0.5f}}, // 右上後
        {{-0.5f,  0.5f, -0.5f},  {-0.5f,  0.5f, -0.5f}}  // 左上後
    };

    // 2. 定義索引
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0, // 前面
        1, 5, 6, 6, 2, 1, // 右面
        7, 6, 5, 5, 4, 7, // 背面
        4, 0, 3, 3, 7, 4, // 左面
        4, 5, 1, 1, 0, 4, // 底面
        3, 2, 6, 6, 7, 3  // 頂面
    };
    m_IndexCount = static_cast<int>(indices.size());

    // 3. 建立 OpenGL 物件
    glCreateVertexArrays(1, &m_MeshVAO);

    // VBO (頂點資料)
    glCreateBuffers(1, &m_MeshVBO);
    glNamedBufferStorage(m_MeshVBO, vertices.size() * sizeof(Vertex), vertices.data(), 0);

    // EBO (索引資料)
    glCreateBuffers(1, &m_MeshEBO);
    glNamedBufferStorage(m_MeshEBO, indices.size() * sizeof(unsigned int), indices.data(), 0);

    // 4. 連結 VAO
    glVertexArrayVertexBuffer(m_MeshVAO, 0, m_MeshVBO, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(m_MeshVAO, m_MeshEBO);

    // 設定屬性 0: Position (vec3)
    glEnableVertexArrayAttrib(m_MeshVAO, 0);
    glVertexArrayAttribFormat(m_MeshVAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribBinding(m_MeshVAO, 0, 0);

    // 設定屬性 1: Normal (vec3)
    glEnableVertexArrayAttrib(m_MeshVAO, 1);
    glVertexArrayAttribFormat(m_MeshVAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(m_MeshVAO, 1, 0);
}