#pragma once
#include <glad/glad.h>
#include <cstddef>
#include <vector>

class Buffer {
public:
    // size: bytes 總大小
    // data: 初始數據指標 (可為 nullptr)
    Buffer(GLsizeiptr size, const void* data, GLbitfield flags);
    ~Buffer();

    // 將 Buffer 綁定到 Shader 的 binding point (例如 binding = 0)
    void bindBase(GLuint index) const;

    // 讀取數據回 CPU (用於 Debug)
    void* map(GLenum access);
    void unmap();

    GLuint getID() const { return m_ID; }

private:
    GLuint m_ID;
};