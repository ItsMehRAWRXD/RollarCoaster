#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

// Forward declarations
class Shader;
class Texture;
class Framebuffer;
class Mesh;

// G-buffer layout
struct GBuffer {
    GLuint fbo;
    GLuint albedo;
    GLuint normal;
    GLuint material;
    GLuint depth;
    int width, height;
};

// Deferred renderer for modern OpenGL pipeline
class DeferredRenderer {
public:
    DeferredRenderer();
    ~DeferredRenderer();

    bool Initialize(int width, int height);
    void Shutdown();
    void Resize(int width, int height);

    // G-buffer operations
    void BeginGBuffer();
    void EndGBuffer();
    void RenderGBuffer(std::shared_ptr<Mesh> mesh, const glm::mat4& model);

    // Lighting pass
    void BeginLighting();
    void EndLighting();
    void RenderLighting();

    // Post-processing
    void BeginPostFX();
    void EndPostFX();
    void RenderPostFX();

    // Final composition
    void RenderToScreen();

    // Getters
    std::shared_ptr<GBuffer> GetGBuffer() const { return m_gbuffer; }
    std::shared_ptr<Texture> GetHDRTexture() const { return m_hdrTexture; }
    std::shared_ptr<Texture> GetLDRTexture() const { return m_ldrTexture; }

private:
    void CreateGBuffer();
    void CreateHDRFramebuffer();
    void CreatePostFXFramebuffer();
    void CreateScreenQuad();
    void DestroyResources();

    // G-buffer
    std::shared_ptr<GBuffer> m_gbuffer;
    
    // HDR framebuffer
    GLuint m_hdrFBO;
    std::shared_ptr<Texture> m_hdrTexture;
    
    // Post-FX framebuffer
    GLuint m_postFBO;
    std::shared_ptr<Texture> m_ldrTexture;
    
    // Screen quad for fullscreen passes
    GLuint m_screenVAO;
    GLuint m_screenVBO;
    
    // Shaders
    std::shared_ptr<Shader> m_gbufferShader;
    std::shared_ptr<Shader> m_lightingShader;
    std::shared_ptr<Shader> m_toneMapShader;
    std::shared_ptr<Shader> m_bloomShader;
    std::shared_ptr<Shader> m_ssaoShader;
    
    // Uniform buffers
    GLuint m_cameraUBO;
    GLuint m_lightingUBO;
    
    // Settings
    int m_width, m_height;
    bool m_initialized;
    
    // Post-FX settings
    float m_exposure;
    bool m_useACES;
    bool m_enableBloom;
    bool m_enableSSAO;
};