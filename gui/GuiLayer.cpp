#include "GuiLayer.hpp"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


GuiLayer::GuiLayer(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    setStyle();

    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init("#version 450");
}

GuiLayer::~GuiLayer() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GuiLayer::setStyle() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 5.0f;
    ImFontConfig cfg;
    cfg.SizePixels = 22.0f;
    io.Fonts->AddFontFromFileTTF("assets/TsukuAOldMin_Pr6_E.ttf", 22.0f, &cfg);
}

void GuiLayer::begin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GuiLayer::end() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiLayer::renderUI(ParticleSystem& particleSystem, float& mouseStrength, bool uiMode) {
    // 1. 準心
    if (!uiMode) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        drawList->AddLine(ImVec2(center.x - 10, center.y), ImVec2(center.x + 10, center.y), IM_COL32(255, 255, 255, 150), 2.0f);
        drawList->AddLine(ImVec2(center.x, center.y - 10), ImVec2(center.x, center.y + 10), IM_COL32(255, 255, 255, 150), 2.0f);
    }

    // 2. HUD
    ImGuiWindowFlags hudFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs;
    ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
    float PAD = 20.0f;
    // 設定位置在右下角
    ImGui::SetNextWindowPos(ImVec2(viewportSize.x - PAD, viewportSize.y - PAD), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    if (ImGui::Begin("Spell HUD", nullptr, hudFlags)) {

        float strength = particleSystem.Props.attractorStrength;

        ImGui::SetWindowFontScale(1.2f); 

        if (!uiMode) {
            if (abs(strength) > 180.0f) {
                ImGui::TextColored(ImVec4(0.8f, 0.2f, 1.0f, 1.0f), u8"虛式 · 茈");
                ImGui::TextColored(ImVec4(0.8f, 0.2f, 1.0f, 1.0f), u8"Hollow Technique: Purple");
            }
            else if (strength > 0.0f) {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), u8"術式順轉 · 蒼");
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), u8"Cursed Technique Lapse: Blue");
            }
            else if (strength < 0.0f) {
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), u8"術式反轉 · 赫");
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), u8"Cursed Technique Reversal: Red");
            }
            else {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.5f), u8"無下限咒術");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 0.5f), u8"Neutral Limitless");
            }
        }
        else {
            ImGui::TextColored(ImVec4(1, 1, 1, 0.5), "System Config Active");
        }
    }
    ImGui::End();

    // 3. 控制面板
    if (uiMode) {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Particle Control");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

        // Reset button
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
        if (ImGui::Button("RESET (R)", ImVec2(-1.0f, 30.0f))) {
            particleSystem.reset();
        }
        ImGui::PopStyleColor();

        ImGui::Separator();
        ImGui::SliderFloat("Force", &mouseStrength, 10.0f, 200.0f);
        ImGui::SliderFloat("Size", &particleSystem.Props.pointScale, 100.0f, 5000.0f);
        ImGui::SliderFloat("Gravity", &particleSystem.Props.gravity.y, -20.0f, 20.0f);

        ImGui::End();
    }
}