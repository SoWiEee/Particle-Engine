#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "../app/ParticleSystem.hpp"

class GuiLayer {
public:
    GuiLayer(GLFWwindow* window);
    ~GuiLayer();

    void begin(); // NewFrame
    void end();   // Render

    void renderUI(ParticleSystem& particleSystem, float& mouseStrength, bool uiMode);

private:
    void setStyle();
    ImFont* m_GameFont = nullptr;
};