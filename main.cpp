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

float lastX = 640.0f, lastY = 360.0f;
bool firstMouse = true;
bool uiMode = false;
bool lastInsertState = false;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (uiMode) return;

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos);

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main() {
    try {
        Window window(1280, 720, "GPU Particle System - Final Stage");

        glfwSetCursorPosCallback(window.getNativeWindow(), mouse_callback);
        glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // ImGui init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window.getNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 450");

        ParticleSystem particleSystem(1000000);
        float mouseStrength = 80.0f;

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

        while (!window.shouldClose()) {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            window.pollEvents();
            GLFWwindow* nativeWin = window.getNativeWindow();

            // input handle
            if (glfwGetKey(nativeWin, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(nativeWin, true);

            // switch UI mode
            bool currentInsertState = glfwGetKey(window.getNativeWindow(), GLFW_KEY_INSERT) == GLFW_PRESS;
            if (currentInsertState && !lastInsertState) {
                uiMode = !uiMode; // 切換模式

                if (uiMode) {
                    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                else {
                    glfwSetInputMode(window.getNativeWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    firstMouse = true;
                }
            }
			lastInsertState = currentInsertState;   // update state

            if (!uiMode) {
                // 相機移動 (WASD)
                if (glfwGetKey(nativeWin, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);

                // 滑鼠狀態
                bool leftClick = glfwGetMouseButton(nativeWin, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
                bool rightClick = glfwGetMouseButton(nativeWin, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;

                // 預設重力 (恢復正常)
                glm::vec3 normalGravity(0.0f, -9.8f, 0.0f);

                // [虛式·茈 (Hollow Purple)]：同時按住左鍵和右鍵
                // 條件：強度極大 (200)，無重力，紫色
                if (leftClick && rightClick) {
                    particleSystem.Props.attractorStrength = 250.0f; // 強度全開
                    particleSystem.Props.attractorPos = camera.Position + camera.Front * 25.0f; // 推遠一點
                    particleSystem.Props.gravity = glm::vec3(0.0f); // 消除重力，讓粒子懸浮
                }
                // [術式順轉·蒼 (Blue)]：僅按住左鍵
                // 條件：正向強度，正常重力(或微重力)，藍色螺旋
                else if (leftClick) {
                    particleSystem.Props.attractorStrength = mouseStrength;
                    particleSystem.Props.attractorPos = camera.Position + camera.Front * 20.0f;
                    particleSystem.Props.gravity = normalGravity;
                }
                // [術式反轉·赫 (Red)]：僅按住右鍵
                // 條件：負向強度，正常重力，紅色亂流
                else if (rightClick) {
                    particleSystem.Props.attractorStrength = -mouseStrength * 1.2f; // 斥力稍微加強
                    particleSystem.Props.attractorPos = camera.Position + camera.Front * 15.0f;
                    particleSystem.Props.gravity = normalGravity;
                }
                // [無術式狀態]
                else {
                    particleSystem.Props.attractorStrength = 0.0f;
                    particleSystem.Props.gravity = normalGravity;
                }
            }

            // 渲染流程
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 1. 更新與繪製粒子
            particleSystem.onUpdate(deltaTime, currentFrame);
            particleSystem.onRender(camera);

            // 2. 繪製 ImGui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (uiMode) {
                ImGui::Begin("Particle Control");
                ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Count: %d", particleSystem.Props.particleCount);
                ImGui::Separator();

                ImGui::Text("Interaction Force");
                ImGui::SliderFloat("Force Strength", &mouseStrength, 10.0f, 200.0f);
                ImGui::Separator();

                ImGui::DragFloat3("Gravity", &particleSystem.Props.gravity.x, 0.1f);
                ImGui::SliderFloat("Speed", &particleSystem.Props.emitSpeed, 0.0f, 50.0f);
                ImGui::SliderFloat("Size", &particleSystem.Props.pointScale, 100.0f, 3000.0f);
                ImGui::SliderFloat("Bounce", &particleSystem.Props.bounce, 0.0f, 1.5f);
                // ImGui::SliderFloat("Respawn Height", &particleSystem.Props.respawnHeight, -50.0f, 0.0f);

                ImGui::Text("Hold Left Mouse: Attract");
                ImGui::Text("Hold Right Mouse: Repulse");
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            window.swapBuffers();
        }

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