#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool shouldClose() const { return glfwWindowShouldClose(m_Window); }
    void swapBuffers() { glfwSwapBuffers(m_Window); }
    void pollEvents() { glfwPollEvents(); }

    // 取得原始指標 (給 ImGui 用)
    GLFWwindow* getNativeWindow() const { return m_Window; }

    int getWidth() const { return m_Width; }
    int getHeight() const { return m_Height; }

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;

    void initDebug(); // error callback
};