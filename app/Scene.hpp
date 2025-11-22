#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "../core/Shader.hpp"
#include "../core/Camera.hpp"

class Scene {
public:
    Scene();
    ~Scene();

    void onRender(const Camera& camera, float boundarySize);

private:
    GLuint m_VAO, m_VBO;
    std::unique_ptr<Shader> m_GridShader;

    void initPlaneMesh();
};