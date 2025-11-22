#include "Buffer.hpp"
#include <iostream>

Buffer::Buffer(GLsizeiptr size, const void* data, GLbitfield flags) {
    glCreateBuffers(1, &m_ID);
    glNamedBufferStorage(m_ID, size, data, flags);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &m_ID);
}

void Buffer::bindBase(GLuint index) const {
    // 將此 Buffer 綁定到 SSBO binding point
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_ID);
}

void* Buffer::map(GLenum access) {
    return glMapNamedBuffer(m_ID, access);
}

void Buffer::unmap() {
    glUnmapNamedBuffer(m_ID);
}