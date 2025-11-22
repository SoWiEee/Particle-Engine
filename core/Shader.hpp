#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    // for Vertex + Fragment
    Shader(const std::string& vertPath, const std::string& fragPath);

    // for Compute
    Shader(const std::string& compPath);

    ~Shader();

    void use() const;
    GLuint getID() const { return m_ID; }

    // Utility functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    GLuint m_ID;

    std::string readFile(const std::string& path);
    GLuint compileShader(const std::string& source, GLenum type);
    void checkCompileErrors(GLuint shader, std::string type);
};