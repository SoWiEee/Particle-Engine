#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    std::string vCode = readFile(vertPath);
    std::string fCode = readFile(fragPath);

    GLuint vertex = compileShader(vCode, GL_VERTEX_SHADER);
    GLuint fragment = compileShader(fCode, GL_FRAGMENT_SHADER);

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    checkCompileErrors(m_ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::Shader(const std::string& compPath) {
    std::string cCode = readFile(compPath);

    GLuint compute = compileShader(cCode, GL_COMPUTE_SHADER);

    m_ID = glCreateProgram();
    glAttachShader(m_ID, compute);
    glLinkProgram(m_ID);
    checkCompileErrors(m_ID, "PROGRAM");

    glDeleteShader(compute);
}

Shader::~Shader() {
    glDeleteProgram(m_ID);
}

void Shader::use() const {
    glUseProgram(m_ID);
}

// Set Uniforms
void Shader::setFloat(const std::string& name, float value) const {
    glProgramUniform1f(m_ID, glGetUniformLocation(m_ID, name.c_str()), value);
}
void Shader::setInt(const std::string& name, int value) const {
    glProgramUniform1i(m_ID, glGetUniformLocation(m_ID, name.c_str()), value);
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glProgramUniform3fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glProgramUniformMatrix4fv(m_ID, glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setBool(const std::string& name, bool value) const {
    glProgramUniform1i(m_ID, glGetUniformLocation(m_ID, name.c_str()), (int)value);
}


std::string Shader::readFile(const std::string& path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        return stream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << path << std::endl;
        return "";
    }
}

GLuint Shader::compileShader(const std::string& source, GLenum type) {
    const char* src = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, "SHADER");
    return shader;
}

void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}