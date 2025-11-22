#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <functional>
#include "core/Window.hpp"
#include "core/Shader.hpp"

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main() {
    try {
        Window window(1280, 720, "GPU Particles - Stage 2");

        Shader computeShader("assets/shaders/particle.comp");

        std::cout << "Compute Shader compiled successfully! ID: " << computeShader.getID() << std::endl;

        while (!window.shouldClose()) {
            window.pollEvents();
            if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window.getNativeWindow(), true);

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            window.swapBuffers();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}