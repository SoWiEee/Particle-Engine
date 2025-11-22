#include "Scene.hpp"
#include <vector>

Scene::Scene() {
    // 1. 初始化平面網格
    initPlaneMesh();

    // 2. 載入 Grid Shader (稍後建立)
    m_GridShader = std::make_unique<Shader>("assets/shaders/grid.vert", "assets/shaders/grid.frag");
}

Scene::~Scene() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Scene::initPlaneMesh() {
    // 一個簡單的平面 (兩個三角形)
    // X, Y, Z
    float vertices[] = {
         1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f, -1.0f,

         1.0f, 0.0f,  1.0f,
        -1.0f, 0.0f, -1.0f,
         1.0f, 0.0f, -1.0f
    };

    glCreateVertexArrays(1, &m_VAO);
    glCreateBuffers(1, &m_VBO);

    glNamedBufferStorage(m_VBO, sizeof(vertices), vertices, 0);

    glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, 3 * sizeof(float));
    glEnableVertexArrayAttrib(m_VAO, 0);
    glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(m_VAO, 0, 0);
}

void Scene::onRender(const Camera& camera, float boundarySize) {
    glEnable(GL_DEPTH_TEST);

    m_GridShader->use();
    m_GridShader->setMat4("view", camera.GetViewMatrix());
    m_GridShader->setMat4("projection", camera.GetProjectionMatrix(1280.0f, 720.0f)); // 解析度可改成動態

    // 傳入相機位置與遠近平面，用於計算漸層消失 (Fading)
    m_GridShader->setVec3("cameraPos", camera.Position);
    m_GridShader->setFloat("farPlane", 2000.0f); // 視線最遠距離
    m_GridShader->setFloat("boundarySize", boundarySize); // 用於限制網格範圍

    glBindVertexArray(m_VAO);

    // --- 1. 繪製地板 ---
    glm::mat4 model = glm::mat4(1.0f);
    // 移到物理地板的高度 (記得 update.comp 裡 floorY 是 -8.0)
    model = glm::translate(model, glm::vec3(0.0f, -8.0f, 0.0f));
    // 放大到涵蓋整個邊界 (例如邊界是 80，我們就放大到 160)
    model = glm::scale(model, glm::vec3(boundarySize * 2.0f));

    m_GridShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // --- 2. (選用) 繪製天花板 ---
    // 只要把 Y 移到上方即可，看你需不需要封閉空間感
     model = glm::mat4(1.0f);
     model = glm::translate(model, glm::vec3(0.0f, boundarySize, 0.0f));
     model = glm::scale(model, glm::vec3(boundarySize * 2.0f));
     m_GridShader->setMat4("model", model);
     glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}