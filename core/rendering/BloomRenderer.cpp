#include "BloomRenderer.hpp"
#include <iostream>

BloomRenderer::BloomRenderer(int width, int height) : m_Width(width), m_Height(height) {
    m_BlurShader = std::make_unique<Shader>("assets/shaders/blur.vert", "assets/shaders/blur.frag");
    m_FinalShader = std::make_unique<Shader>("assets/shaders/final.vert", "assets/shaders/final.frag");

    initFramebuffers();
    initQuad();
}

BloomRenderer::~BloomRenderer() {
    glDeleteFramebuffers(1, &m_HdrFBO);
    glDeleteFramebuffers(2, m_PingPongFBO);
    glDeleteVertexArrays(1, &m_QuadVAO);
}

void BloomRenderer::resize(int width, int height) {
    m_Width = width;
    m_Height = height;
    // 簡單粗暴：刪掉重建立
    glDeleteFramebuffers(1, &m_HdrFBO);
    glDeleteTextures(1, &m_HdrColorBuffer);
    glDeleteRenderbuffers(1, &m_RboDepth);
    glDeleteFramebuffers(2, m_PingPongFBO);
    glDeleteTextures(2, m_PingPongColorbuffers);
    initFramebuffers();
}

void BloomRenderer::initFramebuffers() {
    // 1. 建立主 HDR FBO
    glGenFramebuffers(1, &m_HdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_HdrFBO);

    // 建立浮點顏色緩衝區 (GL_RGBA16F 是關鍵，支援 > 1.0 的顏色)
    glGenTextures(1, &m_HdrColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_HdrColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_HdrColorBuffer, 0);

    // 深度緩衝 (RBO)
    glGenRenderbuffers(1, &m_RboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "HDR Framebuffer not complete!" << std::endl;

    // 2. 建立 Ping-Pong FBOs (用於反覆模糊)
    glGenFramebuffers(2, m_PingPongFBO);
    glGenTextures(2, m_PingPongColorbuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_PingPongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_PingPongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PingPongColorbuffers[i], 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BloomRenderer::initQuad() {
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void BloomRenderer::bindForWriting() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_HdrFBO);
    glViewport(0, 0, m_Width, m_Height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 純黑背景
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BloomRenderer::renderBloom(float exposure) {
    bool horizontal = true, first_iteration = true;

    // 1. 高斯模糊處理 (Ping-Pong Blur)
    // 我們進行 10 次迭代 (5次橫向 + 5次縱向)，次數越多越糊越美
    int amount = 10;
    m_BlurShader->use();
    glBindVertexArray(m_QuadVAO);

    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_PingPongFBO[horizontal]);
        m_BlurShader->setInt("horizontal", horizontal);

        // 第一次從 HDR FBO 讀取，之後從另一個 PingPong FBO 讀取
        glBindTexture(GL_TEXTURE_2D, first_iteration ? m_HdrColorBuffer : m_PingPongColorbuffers[!horizontal]);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }

    // 2. Tone Mapping
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_Width, m_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_FinalShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_HdrColorBuffer); // 原始 HDR 場景
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_PingPongColorbuffers[!horizontal]); // 模糊後的泛光

    m_FinalShader->setInt("scene", 0);
    m_FinalShader->setInt("bloomBlur", 1);
    m_FinalShader->setFloat("exposure", exposure);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}