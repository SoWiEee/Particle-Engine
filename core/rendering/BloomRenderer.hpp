#pragma once
#include <glad/glad.h>
#include <vector>
#include <memory>
#include "..//Shader.hpp"

class BloomRenderer {
public:
    BloomRenderer(int width, int height);
    ~BloomRenderer();

    // 當視窗大小改變時呼叫
    void resize(int width, int height);

    // 1. 在渲染場景前呼叫：綁定 HDR Buffer
    void bindForWriting();

    // 2. 在渲染場景後呼叫：進行模糊處理與最終合成
    void renderBloom(float exposure);

private:
    int m_Width, m_Height;

    // 主要的 HDR FBO (場景畫在這裡)
    GLuint m_HdrFBO;
    GLuint m_HdrColorBuffer; // 浮點數紋理 GL_RGBA16F
    GLuint m_RboDepth;

    // 模糊用的雙緩衝 FBO (Ping-Pong)
    GLuint m_PingPongFBO[2];
    GLuint m_PingPongColorbuffers[2];

    // 繪製全螢幕四邊形用的 VAO
    GLuint m_QuadVAO = 0;
    GLuint m_QuadVBO;

    // Shaders
    std::unique_ptr<Shader> m_BlurShader;
    std::unique_ptr<Shader> m_FinalShader;

    void initFramebuffers();
    void initQuad();
};