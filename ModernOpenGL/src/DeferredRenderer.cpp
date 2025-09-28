#include "DeferredRenderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include <iostream>

DeferredRenderer::DeferredRenderer() 
    : m_gbuffer(nullptr)
    , m_hdrFBO(0)
    , m_postFBO(0)
    , m_screenVAO(0)
    , m_screenVBO(0)
    , m_cameraUBO(0)
    , m_lightingUBO(0)
    , m_width(1920)
    , m_height(1080)
    , m_initialized(false)
    , m_exposure(1.0f)
    , m_useACES(true)
    , m_enableBloom(true)
    , m_enableSSAO(true)
{
}

DeferredRenderer::~DeferredRenderer() {
    Shutdown();
}

bool DeferredRenderer::Initialize(int width, int height) {
    m_width = width;
    m_height = height;
    
    // Create G-buffer
    CreateGBuffer();
    
    // Create HDR framebuffer
    CreateHDRFramebuffer();
    
    // Create post-FX framebuffer
    CreatePostFXFramebuffer();
    
    // Create screen quad
    CreateScreenQuad();
    
    // Create shaders
    m_gbufferShader = std::make_shared<Shader>();
    if (!m_gbufferShader->Load(Shader::GetGBufferVertexShader(), Shader::GetGBufferFragmentShader())) {
        std::cerr << "Failed to create G-buffer shader" << std::endl;
        return false;
    }
    
    m_lightingShader = std::make_shared<Shader>();
    if (!m_lightingShader->Load(Shader::GetHDRVertexShader(), Shader::GetLightingFragmentShader())) {
        std::cerr << "Failed to create lighting shader" << std::endl;
        return false;
    }
    
    m_toneMapShader = std::make_shared<Shader>();
    if (!m_toneMapShader->Load(Shader::GetHDRVertexShader(), Shader::GetACESToneMapShader())) {
        std::cerr << "Failed to create tone mapping shader" << std::endl;
        return false;
    }
    
    // Create uniform buffers
    glGenBuffers(1, &m_cameraUBO);
    glGenBuffers(1, &m_lightingUBO);
    
    // Setup uniform buffer bindings
    m_gbufferShader->SetUniformBlock("Camera", 0);
    m_lightingShader->SetUniformBlock("Camera", 0);
    m_lightingShader->SetUniformBlock("Lighting", 1);
    
    m_initialized = true;
    std::cout << "Deferred renderer initialized successfully!" << std::endl;
    return true;
}

void DeferredRenderer::Shutdown() {
    DestroyResources();
    m_initialized = false;
}

void DeferredRenderer::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    
    // Recreate G-buffer with new dimensions
    if (m_gbuffer) {
        glDeleteFramebuffers(1, &m_gbuffer->fbo);
        glDeleteTextures(1, &m_gbuffer->albedo);
        glDeleteTextures(1, &m_gbuffer->normal);
        glDeleteTextures(1, &m_gbuffer->material);
        glDeleteTextures(1, &m_gbuffer->depth);
    }
    
    CreateGBuffer();
    CreateHDRFramebuffer();
    CreatePostFXFramebuffer();
}

void DeferredRenderer::BeginGBuffer() {
    if (!m_gbuffer) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer->fbo);
    glViewport(0, 0, m_gbuffer->width, m_gbuffer->height);
    
    // Clear G-buffer
    GLfloat clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, clearColor);
    glClearBufferfv(GL_COLOR, 1, clearColor);
    glClearBufferfv(GL_COLOR, 2, clearColor);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Use G-buffer shader
    m_gbufferShader->Use();
}

void DeferredRenderer::EndGBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::RenderGBuffer(std::shared_ptr<Mesh> mesh, const glm::mat4& model) {
    if (!mesh || !m_gbufferShader) return;
    
    // Set model matrix
    m_gbufferShader->SetMat4("uModel", model);
    
    // Render mesh
    mesh->Render();
}

void DeferredRenderer::BeginLighting() {
    if (!m_hdrFBO) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
    glViewport(0, 0, m_width, m_height);
    
    // Clear HDR buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Disable depth testing for fullscreen pass
    glDisable(GL_DEPTH_TEST);
    
    // Use lighting shader
    m_lightingShader->Use();
    
    // Bind G-buffer textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->albedo);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->normal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->material);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->depth);
    
    // Set texture uniforms
    m_lightingShader->SetInt("gAlbedo", 0);
    m_lightingShader->SetInt("gNormal", 1);
    m_lightingShader->SetInt("gMaterial", 2);
    m_lightingShader->SetInt("gDepth", 3);
}

void DeferredRenderer::EndLighting() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::RenderLighting() {
    // Render fullscreen quad
    glBindVertexArray(m_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void DeferredRenderer::BeginPostFX() {
    if (!m_postFBO) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_postFBO);
    glViewport(0, 0, m_width, m_height);
    
    // Clear post-FX buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Disable depth testing
    glDisable(GL_DEPTH_TEST);
}

void DeferredRenderer::EndPostFX() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::RenderPostFX() {
    // Tone mapping pass
    m_toneMapShader->Use();
    
    // Bind HDR texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdrTexture->GetID());
    
    // Set uniforms
    m_toneMapShader->SetInt("uHDRTexture", 0);
    m_toneMapShader->SetFloat("uExposure", m_exposure);
    m_toneMapShader->SetBool("uUseACES", m_useACES);
    
    // Render fullscreen quad
    glBindVertexArray(m_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void DeferredRenderer::RenderToScreen() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_width, m_height);
    
    // Clear screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render final result
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ldrTexture->GetID());
    
    // Simple screen shader
    glBindVertexArray(m_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void DeferredRenderer::CreateGBuffer() {
    m_gbuffer = std::make_shared<GBuffer>();
    m_gbuffer->width = m_width;
    m_gbuffer->height = m_height;
    
    // Create FBO
    glGenFramebuffers(1, &m_gbuffer->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_gbuffer->fbo);
    
    // Create albedo texture (RGBA8)
    glGenTextures(1, &m_gbuffer->albedo);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->albedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gbuffer->albedo, 0);
    
    // Create normal texture (RGBA16F)
    glGenTextures(1, &m_gbuffer->normal);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gbuffer->normal, 0);
    
    // Create material texture (RGBA8)
    glGenTextures(1, &m_gbuffer->material);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->material);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gbuffer->material, 0);
    
    // Create depth texture
    glGenTextures(1, &m_gbuffer->depth);
    glBindTexture(GL_TEXTURE_2D, m_gbuffer->depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gbuffer->depth, 0);
    
    // Set draw buffers
    GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "G-buffer framebuffer not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::CreateHDRFramebuffer() {
    // Create HDR FBO
    glGenFramebuffers(1, &m_hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_hdrFBO);
    
    // Create HDR texture (R11G11B10F)
    m_hdrTexture = std::make_shared<Texture>();
    m_hdrTexture->Create(m_width, m_height, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT, "HDR");
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_hdrTexture->GetID(), 0);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "HDR framebuffer not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::CreatePostFXFramebuffer() {
    // Create post-FX FBO
    glGenFramebuffers(1, &m_postFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_postFBO);
    
    // Create LDR texture (RGBA8)
    m_ldrTexture = std::make_shared<Texture>();
    m_ldrTexture->Create(m_width, m_height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, "LDR");
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ldrTexture->GetID(), 0);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Post-FX framebuffer not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::CreateScreenQuad() {
    // Create screen quad vertices
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glGenVertexArrays(1, &m_screenVAO);
    glGenBuffers(1, &m_screenVBO);
    
    glBindVertexArray(m_screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

void DeferredRenderer::DestroyResources() {
    if (m_gbuffer) {
        glDeleteFramebuffers(1, &m_gbuffer->fbo);
        glDeleteTextures(1, &m_gbuffer->albedo);
        glDeleteTextures(1, &m_gbuffer->normal);
        glDeleteTextures(1, &m_gbuffer->material);
        glDeleteTextures(1, &m_gbuffer->depth);
    }
    
    if (m_hdrFBO) {
        glDeleteFramebuffers(1, &m_hdrFBO);
    }
    
    if (m_postFBO) {
        glDeleteFramebuffers(1, &m_postFBO);
    }
    
    if (m_screenVAO) {
        glDeleteVertexArrays(1, &m_screenVAO);
    }
    
    if (m_screenVBO) {
        glDeleteBuffers(1, &m_screenVBO);
    }
    
    if (m_cameraUBO) {
        glDeleteBuffers(1, &m_cameraUBO);
    }
    
    if (m_lightingUBO) {
        glDeleteBuffers(1, &m_lightingUBO);
    }
}