#include "Mesh.h"
#include <iostream>

Mesh::Mesh() 
    : m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_vertexCount(0)
    , m_indexCount(0)
    , m_name("")
    , m_initialized(false)
{
}

Mesh::~Mesh() {
    Destroy();
}

bool Mesh::Load(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    if (m_initialized) {
        Destroy();
    }
    
    m_vertexCount = vertices.size() / 8; // Assuming 8 floats per vertex (pos, normal, uv)
    m_indexCount = indices.size();
    
    // Generate buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    // Bind VAO
    glBindVertexArray(m_VAO);
    
    // Bind and set vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Bind and set index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Set vertex attributes
    // Position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    
    // Normal (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    
    // Texture coordinates (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    // Unbind VAO
    glBindVertexArray(0);
    
    m_initialized = true;
    return true;
}

void Mesh::Render() {
    if (!m_initialized) return;
    
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::RenderInstanced(int instanceCount) {
    if (!m_initialized) return;
    
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0, instanceCount);
    glBindVertexArray(0);
}

void Mesh::Destroy() {
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_EBO) {
        glDeleteBuffers(1, &m_EBO);
        m_EBO = 0;
    }
    
    m_vertexCount = 0;
    m_indexCount = 0;
    m_initialized = false;
}