#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Mesh {
public:
    Mesh();
    ~Mesh();

    bool Load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    void Render();
    void RenderInstanced(int instanceCount);
    void Destroy();
    
    // Getters
    GLuint GetVAO() const { return m_VAO; }
    GLuint GetVBO() const { return m_VBO; }
    GLuint GetEBO() const { return m_EBO; }
    size_t GetVertexCount() const { return m_vertexCount; }
    size_t GetIndexCount() const { return m_indexCount; }
    size_t GetTriangleCount() const { return m_indexCount / 3; }
    bool IsValid() const { return m_initialized; }
    
    // Setters
    void SetName(const std::string& name) { m_name = name; }
    const std::string& GetName() const { return m_name; }

private:
    GLuint m_VAO, m_VBO, m_EBO;
    size_t m_vertexCount;
    size_t m_indexCount;
    std::string m_name;
    bool m_initialized;
};