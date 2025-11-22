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
#include "core/InputHandler.hpp"
#include "gui/GuiLayer.hpp"
#include "app/ParticleSystem.hpp"
#include "app/Scene.hpp"
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
        Window window(1280, 720, "GPU Particle System (Refactored)");
        InputHandler input(window.getNativeWindow(), camera);
        GuiLayer gui(window.getNativeWindow());

        ParticleSystem particleSystem(500000);
        Scene scene;

        glEnable(GL_PROGRAM_POINT_SIZE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        float mouseStrength = 80.0f;

        while (!window.shouldClose()) {
            float currentFrame = (float)glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            window.pollEvents();

            // 1. WASD, Reset
            input.processInput(deltaTime);

            // Reset check
            if (input.shouldReset()) {
                particleSystem.reset();
                input.clearResetFlag();
            }

            // 2. 處理術式邏輯 (Gojo Logic)
            GLFWwindow* nativeWin = window.getNativeWindow();
            if (!input.isUiMode()) {
                bool left = glfwGetMouseButton(nativeWin, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
                bool right = glfwGetMouseButton(nativeWin, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS;

                if (left && right) {
                    particleSystem.Props.attractorStrength = 250.0f; // 茈
                }
                else if (left) {
                    particleSystem.Props.attractorStrength = mouseStrength; // 蒼
                }
                else if (right) {
                    particleSystem.Props.attractorStrength = -mouseStrength; // 赫
                }
                else {
                    particleSystem.Props.attractorStrength = 0.0f;
                }

                // 更新位置
                if (particleSystem.Props.attractorStrength != 0.0f) {
                    particleSystem.Props.attractorPos = camera.Position + camera.Front * 20.0f;
                }
            }

            // 3. render
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // draw scene
            scene.onRender(camera, particleSystem.Props.boundarySize);

            particleSystem.onUpdate(deltaTime, currentFrame);
            particleSystem.onRender(camera);

            // 4. draw GUI
            gui.begin();
            gui.renderUI(particleSystem, mouseStrength, input.isUiMode());
            gui.end();

            window.swapBuffers();
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}