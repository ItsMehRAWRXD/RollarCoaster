#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>
#include <memory>

class Shader {
public:
    Shader();
    ~Shader();

    bool Load(const std::string& vertexSource, const std::string& fragmentSource);
    bool Load(const std::string& vertexSource, const std::string& geometrySource, const std::string& fragmentSource);
    bool LoadCompute(const std::string& computeSource);
    
    void Use();
    void Unuse();
    
    // Uniform setters
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat2(const std::string& name, const glm::mat2& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    
    // Uniform block setters
    void SetUniformBlock(const std::string& name, GLuint binding);
    
    // Getters
    GLuint GetProgram() const { return m_program; }
    bool IsValid() const { return m_program != 0; }
    
    // Shader source generators
    static std::string GetDefaultVertexShader();
    static std::string GetDefaultFragmentShader();
    static std::string GetPBRVertexShader();
    static std::string GetPBRFragmentShader();
    static std::string GetHDRVertexShader();
    static std::string GetHDRFragmentShader();
    static std::string GetACESToneMapShader();
    static std::string GetGBufferVertexShader();
    static std::string GetGBufferFragmentShader();
    static std::string GetLightingVertexShader();
    static std::string GetLightingFragmentShader();
    static std::string GetSSAOVertexShader();
    static std::string GetSSAOFragmentShader();
    static std::string GetBloomVertexShader();
    static std::string GetBloomFragmentShader();
    static std::string GetTAAShader();

private:
    GLuint m_program;
    std::unordered_map<std::string, GLint> m_uniformLocations;
    
    GLuint CompileShader(const std::string& source, GLenum type);
    GLint GetUniformLocation(const std::string& name);
    void CheckCompileErrors(GLuint shader, const std::string& type);
};