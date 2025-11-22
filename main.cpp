#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <functional>
#include "core/Window.hpp"
#include "core/Shader.hpp"
#include "core/Buffer.hpp"
#include "core/Types.hpp"
#include "app/ParticleSystem.hpp"
#include <vector>
#include <iomanip>

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main() {
    try {
        Window window(1280, 720, "GPU Particles - Stage 4: Visualization");

        ParticleSystem particleSystem(100000);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // enable Program Point Size
        glEnable(GL_PROGRAM_POINT_SIZE);

        while (!window.shouldClose()) {
            window.pollEvents();
            if (glfwGetKey(window.getNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window.getNativeWindow(), true);

            particleSystem.onUpdate(0.016f, (float)glfwGetTime());

            // render
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            particleSystem.onRender();

            window.swapBuffers();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}