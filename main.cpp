#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <functional>
#include "core/Window.hpp"
#include "core/Shader.hpp"
#include "core/Buffer.hpp"
#include "core/Types.hpp"
#include <vector>
#include <iomanip>

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

int main() {
    try {
        Window window(800, 600, "GPU Particles - Stage 3: Data Verify");
        Shader computeShader("assets/shaders/particle.comp");

        const int PARTICLE_COUNT = 5;
        std::vector<Particle> hostParticles(PARTICLE_COUNT);

		// init particles
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            hostParticles[i].position = glm::vec4(0.0f, 10.0f, 0.0f, 1.0f);
            hostParticles[i].velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            hostParticles[i].color = glm::vec4(1.0f);
        }

        // SSBO -> GPU
        Buffer ssbo(sizeof(Particle) * PARTICLE_COUNT, hostParticles.data(), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);

        // ¸j©w¨ì binding point 0
        ssbo.bindBase(0);

        // GPU compute
        computeShader.use();
        computeShader.setInt("pCount", PARTICLE_COUNT);
        computeShader.setFloat("dt", 0.1f);

        std::cout << "Running simulation on GPU..." << std::endl;

        // Dispatch
        int numGroups = (PARTICLE_COUNT + 127) / 128;
        glDispatchCompute(numGroups, 1, 1);

        // Memory Barrier
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

        Particle* gpuData = (Particle*)ssbo.map(GL_READ_ONLY);

        std::cout << std::fixed << std::setprecision(2);
        for (int i = 0; i < PARTICLE_COUNT; i++) {
            std::cout << "Particle " << i << ": "
                << "Pos(" << gpuData[i].position.x << ", " << gpuData[i].position.y << ", " << gpuData[i].position.z << ") "
                << "Vel(" << gpuData[i].velocity.x << ", " << gpuData[i].velocity.y << ", " << gpuData[i].velocity.z << ")"
                << std::endl;
        }
        ssbo.unmap();

        if (gpuData[0].position.y < 10.0f && gpuData[0].velocity.y < 0.0f) {
            std::cout << "\n[V] SUCCESS! GPU is doing physics!" << std::endl;
        }
        else {
            std::cout << "\n[X] FAILURE! Data didn't change." << std::endl;
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}