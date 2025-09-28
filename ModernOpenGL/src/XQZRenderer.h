#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

// Forward declarations
class Shader;
class Mesh;

// XQZ wireframe renderer for neon-style base geometry
class XQZRenderer {
public:
    XQZRenderer();
    ~XQZRenderer();

    bool Initialize();
    void Shutdown();
    
    // Rendering
    void BeginXQZPass();
    void EndXQZPass();
    void RenderWireframe(std::shared_ptr<Mesh> mesh, const glm::mat4& model);
    void RenderSolid(std::shared_ptr<Mesh> mesh, const glm::mat4& model);
    
    // Settings
    void SetWireframeColor(const glm::vec4& color);
    void SetSolidColor(const glm::vec4& color);
    void SetWireframeEnabled(bool enabled);
    void SetSolidEnabled(bool enabled);
    void SetAlpha(float alpha);
    
    // Getters
    bool IsWireframeEnabled() const { return m_wireframeEnabled; }
    bool IsSolidEnabled() const { return m_solidEnabled; }
    glm::vec4 GetWireframeColor() const { return m_wireframeColor; }
    glm::vec4 GetSolidColor() const { return m_solidColor; }
    std::shared_ptr<Shader> GetWireframeShader() const { return m_wireframeShader; }
    std::shared_ptr<Shader> GetSolidShader() const { return m_solidShader; }

private:
    void CreateShaders();
    void SetupBlending();
    void SetupDepthTesting();
    
    // Shaders
    std::shared_ptr<Shader> m_wireframeShader;
    std::shared_ptr<Shader> m_solidShader;
    
    // Settings
    glm::vec4 m_wireframeColor;
    glm::vec4 m_solidColor;
    bool m_wireframeEnabled;
    bool m_solidEnabled;
    float m_alpha;
    
    // OpenGL state
    bool m_initialized;
    GLenum m_previousPolygonMode;
};