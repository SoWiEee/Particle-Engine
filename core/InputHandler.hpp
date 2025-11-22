#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.hpp"

class InputHandler {
public:
    InputHandler(GLFWwindow* window, Camera& camera);

    void processInput(float dt);

    bool isUiMode() const { return m_UiMode; }

    bool shouldReset() const { return m_ShouldReset; }
    void clearResetFlag() { m_ShouldReset = false; }

private:
    GLFWwindow* m_Window;
    Camera& m_Camera;

    bool m_UiMode = false;
    bool m_FirstMouse = true;
    float m_LastX = 0.0f;
    float m_LastY = 0.0f;

    bool m_ShouldReset = false;

    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CharCallback(GLFWwindow* window, unsigned int codepoint);
};