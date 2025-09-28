#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

// Forward declarations
class Shader;
class Texture;
class Mesh;
class Framebuffer;

// Renderer statistics
struct RenderStats {
    int drawCalls = 0;
    int triangles = 0;
    int vertices = 0;
    float frameTime = 0.0f;
    float gpuTime = 0.0f;
    int textureBinds = 0;
    int shaderBinds = 0;
};

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize(int width, int height);
    void Shutdown();
    void Resize(int width, int height);

    // Rendering state
    void BeginFrame();
    void EndFrame();
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void SetViewport(int x, int y, int width, int height);
    void SetDepthTest(bool enable);
    void SetCullFace(bool enable);
    void SetBlend(bool enable);
    void SetBlendFunc(GLenum src, GLenum dst);

    // Shader management
    std::shared_ptr<Shader> CreateShader(const std::string& vertexSource, 
                                        const std::string& fragmentSource);
    std::shared_ptr<Shader> CreateShader(const std::string& vertexSource,
                                        const std::string& geometrySource,
                                        const std::string& fragmentSource);
    std::shared_ptr<Shader> CreateComputeShader(const std::string& computeSource);
    void UseShader(std::shared_ptr<Shader> shader);

    // Texture management
    std::shared_ptr<Texture> CreateTexture(int width, int height, GLenum internalFormat,
                                          GLenum format, GLenum type, const std::string& name = "");
    std::shared_ptr<Texture> CreateTexture(const std::string& filepath);
    std::shared_ptr<Texture> CreateCubemap(const std::vector<std::string>& faces);
    void BindTexture(std::shared_ptr<Texture> texture, int unit = 0);
    void BindTextureArray(std::shared_ptr<Texture> texture, int unit = 0);

    // Mesh management
    std::shared_ptr<Mesh> CreateMesh(const std::vector<float>& vertices,
                                   const std::vector<unsigned int>& indices);
    void RenderMesh(std::shared_ptr<Mesh> mesh);
    void RenderMeshInstanced(std::shared_ptr<Mesh> mesh, int instanceCount);

    // Framebuffer management
    std::shared_ptr<Framebuffer> CreateFramebuffer(int width, int height);
    void BindFramebuffer(std::shared_ptr<Framebuffer> framebuffer);
    void UnbindFramebuffer();

    // Modern OpenGL features
    void EnableSRGB(bool enable);
    void EnableSeamlessCubemap(bool enable);
    void EnableAnisotropicFiltering(float maxAniso = 16.0f);
    
    // GPU queries
    void BeginQuery(const std::string& name);
    void EndQuery(const std::string& name);
    float GetQueryTime(const std::string& name);

    // Statistics
    const RenderStats& GetStats() const { return m_stats; }
    void ResetStats();

    // Getters
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    GLuint GetDefaultVAO() const { return m_defaultVAO; }

private:
    void CreateDefaultVAO();
    void SetupDebugOutput();
    void CheckGLError(const std::string& operation);

    int m_width, m_height;
    bool m_initialized;
    
    // Default objects
    GLuint m_defaultVAO;
    
    // Current state
    std::shared_ptr<Shader> m_currentShader;
    std::shared_ptr<Framebuffer> m_currentFramebuffer;
    
    // Statistics
    RenderStats m_stats;
    
    // GPU queries
    std::unordered_map<std::string, GLuint> m_queries;
    std::unordered_map<std::string, float> m_queryTimes;
    
    // Resource caches
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaderCache;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureCache;
};