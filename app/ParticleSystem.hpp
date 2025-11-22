class ParticleSystem {
public:
    ParticleSystem(uint32_t count);
    ~ParticleSystem();

    void onUpdate(float dt, float time); // Dispatch Compute Shader
    void onRender(const Camera& camera); // Draw Arrays Instanced
    void onImGuiRender();                // 調整參數面板

private:
    uint32_t m_ParticleCount;

    // 資源管理 (使用 Smart Pointers 自動管理生命週期)
    std::unique_ptr<Buffer> m_SSBO;
    std::unique_ptr<Shader> m_ComputeShader;
    std::unique_ptr<Shader> m_RenderShader;
    GLuint m_VAO; // 即便是繪製點，OpenGL Core Profile 也需要一個空的 VAO

    // 參數控制 (for ImGui)
    struct Properties {
        float gravity = -9.8f;
        float speed = 10.0f;
        glm::vec3 emitterPos = { 0.0f, 5.0f, 0.0f };
    } m_Props;
};