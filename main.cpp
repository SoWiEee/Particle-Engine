#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <functional>
#include "core/Window.hpp"
#include "core/Shader.hpp"
#include "core/Buffer.hpp"
#include "core/Types.hpp"
#include "core/Camera.hpp"
#include "app/ParticleSystem.hpp"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <vector>
#include <iomanip>

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

Camera camera(glm::vec3(0.0f, 10.0f, 40.0f));
float lastX = 640, lastY = 360;
bool firstMouse = true;
bool uiMode = false; // 按 Space 切換：控制相機 <-> 控制 UI

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (uiMode) return; // UI 模式時不轉動相機

    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y 座標相反
    lastX = xpos; lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main() {
    try {
        Window window(1280, 720, "GPU Particles - Stage 5");

        glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
        glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // ImGui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window.getNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 450");

        ParticleSystem particleSystem(50000); // 試試 5萬

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        while (!window.shouldClose()) {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            window.pollEvents();
            GLFWwindow* nativeWin = window.getNativeWindow();

            // 輸入處理
            if (glfwGetKey(nativeWin, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(nativeWin, true);

            // 切換 UI 模式 (按住 Space 顯示滑鼠)
            if (glfwGetKey(nativeWin, GLFW_KEY_SPACE) == GLFW_PRESS) {
                uiMode = true;
                glfwSetInputMode(nativeWin, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else {
                uiMode = false;
                glfwSetInputMode(nativeWin, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            if (!uiMode) {
                if (glfwGetKey(nativeWin, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime); // 降
                if (glfwGetKey(nativeWin, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);   // 升
            }

            // 渲染流程
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 1. 更新與繪製粒子
            particleSystem.onUpdate(deltaTime, currentFrame);

            // 開啟深度測試與混合
            // 注意：渲染半透明粒子時，通常會關閉 Depth Write (glDepthMask(GL_FALSE)) 
            // 但開啟 Depth Test，這樣粒子才不會互擋太嚴重，但會有排序問題 (Addtive Blending 不怕排序)
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive
            particleSystem.onRender(camera);
            glDisable(GL_BLEND);

            // 2. 繪製 ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (uiMode) {
                ImGui::Begin("Particle Control");
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Particles: %d", particleSystem.Props.particleCount);

                ImGui::Separator();
                ImGui::DragFloat3("Gravity", &particleSystem.Props.gravity.x, 0.1f);
                ImGui::SliderFloat("Emit Speed", &particleSystem.Props.emitSpeed, 0.0f, 100.0f);
                ImGui::SliderFloat("Point Scale", &particleSystem.Props.pointScale, 10.0f, 2000.0f);
                ImGui::SliderFloat("Respawn Height", &particleSystem.Props.respawnHeight, -50.0f, 0.0f);

                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            window.swapBuffers();
        }

        // 清理
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}