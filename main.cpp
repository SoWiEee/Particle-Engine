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

        ParticleSystem particleSystem(50000);

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
                if (glfwGetKey(nativeWin, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(FORWARD, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(BACKWARD, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(LEFT, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(RIGHT, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_Q) == GLFW_PRESS) camera.ProcessKeyboard(DOWN, deltaTime);
                if (glfwGetKey(nativeWin, GLFW_KEY_E) == GLFW_PRESS) camera.ProcessKeyboard(UP, deltaTime);

                // 滑鼠互動 (引力場)
                if (glfwGetMouseButton(nativeWin, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                    particleSystem.Props.attractorStrength = 50.0f;
                    particleSystem.Props.attractorPos = camera.Position + camera.Front * 10.0f;
                }
                else if (glfwGetMouseButton(nativeWin, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
                    particleSystem.Props.attractorStrength = -50.0f;
                    particleSystem.Props.attractorPos = camera.Position + camera.Front * 10.0f;
                }
                else {
                    particleSystem.Props.attractorStrength = 0.0f;
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
                ImGui::Text("Particles: %d", particleSystem.Props.particleCount);

                ImGui::Separator();
                ImGui::DragFloat3("Gravity", &particleSystem.Props.gravity.x, 0.1f);
                ImGui::SliderFloat("Bounce", &particleSystem.Props.bounce, 0.0f, 1.5f);
                ImGui::SliderFloat("Speed", &particleSystem.Props.emitSpeed, 0.0f, 50.0f);
                ImGui::SliderFloat("Point Scale", &particleSystem.Props.pointScale, 10.0f, 2000.0f);
                ImGui::SliderFloat("Respawn Height", &particleSystem.Props.respawnHeight, -50.0f, 0.0f);

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