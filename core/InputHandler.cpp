#include "InputHandler.hpp"
#include <backends/imgui_impl_glfw.h>

InputHandler::InputHandler(GLFWwindow* window, Camera& camera)
    : m_Window(window), m_Camera(camera)
{
    glfwSetWindowUserPointer(window, this);

    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCharCallback(window, CharCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // mouse position
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    m_LastX = (float)x;
    m_LastY = (float)y;
}

void InputHandler::processInput(float dt) {
    if (m_UiMode) return;

    if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) m_Camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) m_Camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) m_Camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) m_Camera.ProcessKeyboard(RIGHT, dt);
    if (glfwGetKey(m_Window, GLFW_KEY_E) == GLFW_PRESS) m_Camera.ProcessKeyboard(UP, dt);
    if (glfwGetKey(m_Window, GLFW_KEY_Q) == GLFW_PRESS) m_Camera.ProcessKeyboard(DOWN, dt);

    // Reset 檢測
    if (glfwGetKey(m_Window, GLFW_KEY_R) == GLFW_PRESS) {
        m_ShouldReset = true;
        m_Camera.Reset();
    }
}

void InputHandler::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // 0. 轉發給 ImGui
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);

    // 1. 取回 instance
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (!handler) return;

    // 2. 邏輯處理
    if (handler->m_UiMode) return;

    if (handler->m_FirstMouse) {
        handler->m_LastX = (float)xpos;
        handler->m_LastY = (float)ypos;
        handler->m_FirstMouse = false;
    }

    float xoffset = (float)xpos - handler->m_LastX;
    float yoffset = handler->m_LastY - (float)ypos;
    handler->m_LastX = (float)xpos;
    handler->m_LastY = (float)ypos;

    handler->m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

void InputHandler::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler && !handler->m_UiMode) {
        handler->m_Camera.ProcessMouseScroll((float)yoffset);
    }
}

void InputHandler::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (!handler) return;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

	// switch UI mode
    if (key == GLFW_KEY_INSERT && action == GLFW_PRESS) {
        handler->m_UiMode = !handler->m_UiMode;
        if (handler->m_UiMode) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            handler->m_FirstMouse = true;
        }
    }
}

void InputHandler::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void InputHandler::CharCallback(GLFWwindow* window, unsigned int codepoint) {
    ImGui_ImplGlfw_CharCallback(window, codepoint);
}