#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include <iostream>
#include <chrono>

Renderer::Renderer() 
    : m_width(1920)
    , m_height(1080)
    , m_initialized(false)
    , m_defaultVAO(0)
    , m_currentShader(nullptr)
    , m_currentFramebuffer(nullptr)
{
    m_stats = {};
}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(int width, int height) {
    m_width = width;
    m_height = height;
    
    // Check OpenGL version
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    
    if (major < 4 || (major == 4 && minor < 5)) {
        std::cerr << "OpenGL 4.5+ required, got " << major << "." << minor << std::endl;
        return false;
    }
    
    // Enable modern OpenGL features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Enable sRGB framebuffer for gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    // Enable seamless cubemap sampling
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    // Enable anisotropic filtering
    EnableAnisotropicFiltering(16.0f);
    
    // Create default VAO
    CreateDefaultVAO();
    
    // Setup debug output
    SetupDebugOutput();
    
    m_initialized = true;
    std::cout << "Renderer initialized successfully!" << std::endl;
    return true;
}

void Renderer::Shutdown() {
    // Clean up resources
    m_shaderCache.clear();
    m_textureCache.clear();
    
    if (m_defaultVAO) {
        glDeleteVertexArrays(1, &m_defaultVAO);
        m_defaultVAO = 0;
    }
    
    m_initialized = false;
}

void Renderer::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

void Renderer::BeginFrame() {
    m_stats.drawCalls = 0;
    m_stats.triangles = 0;
    m_stats.vertices = 0;
    m_stats.textureBinds = 0;
    m_stats.shaderBinds = 0;
}

void Renderer::EndFrame() {
    // Update frame timing
    auto now = std::chrono::high_resolution_clock::now();
    static auto lastTime = now;
    auto deltaTime = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;
    
    m_stats.frameTime = deltaTime * 1000.0f; // Convert to milliseconds
}

void Renderer::Clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SetViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void Renderer::SetDepthTest(bool enable) {
    if (enable) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
}

void Renderer::SetCullFace(bool enable) {
    if (enable) {
        glEnable(GL_CULL_FACE);
    } else {
        glDisable(GL_CULL_FACE);
    }
}

void Renderer::SetBlend(bool enable) {
    if (enable) {
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}

void Renderer::SetBlendFunc(GLenum src, GLenum dst) {
    glBlendFunc(src, dst);
}

std::shared_ptr<Shader> Renderer::CreateShader(const std::string& vertexSource, 
                                             const std::string& fragmentSource) {
    return CreateShader(vertexSource, "", fragmentSource);
}

std::shared_ptr<Shader> Renderer::CreateShader(const std::string& vertexSource,
                                             const std::string& geometrySource,
                                             const std::string& fragmentSource) {
    auto shader = std::make_shared<Shader>();
    if (shader->Load(vertexSource, geometrySource, fragmentSource)) {
        return shader;
    }
    return nullptr;
}

std::shared_ptr<Shader> Renderer::CreateComputeShader(const std::string& computeSource) {
    auto shader = std::make_shared<Shader>();
    if (shader->LoadCompute(computeSource)) {
        return shader;
    }
    return nullptr;
}

void Renderer::UseShader(std::shared_ptr<Shader> shader) {
    if (shader && shader != m_currentShader) {
        shader->Use();
        m_currentShader = shader;
        m_stats.shaderBinds++;
    }
}

std::shared_ptr<Texture> Renderer::CreateTexture(int width, int height, GLenum internalFormat,
                                               GLenum format, GLenum type, const std::string& name) {
    auto texture = std::make_shared<Texture>();
    if (texture->Create(width, height, internalFormat, format, type)) {
        texture->SetName(name);
        return texture;
    }
    return nullptr;
}

std::shared_ptr<Texture> Renderer::CreateTexture(const std::string& filepath) {
    auto texture = std::make_shared<Texture>();
    if (texture->LoadFromFile(filepath)) {
        return texture;
    }
    return nullptr;
}

std::shared_ptr<Texture> Renderer::CreateCubemap(const std::vector<std::string>& faces) {
    auto texture = std::make_shared<Texture>();
    if (texture->LoadCubemap(faces)) {
        return texture;
    }
    return nullptr;
}

void Renderer::BindTexture(std::shared_ptr<Texture> texture, int unit) {
    if (texture) {
        texture->Bind(unit);
        m_stats.textureBinds++;
    }
}

void Renderer::BindTextureArray(std::shared_ptr<Texture> texture, int unit) {
    if (texture) {
        texture->BindArray(unit);
        m_stats.textureBinds++;
    }
}

std::shared_ptr<Mesh> Renderer::CreateMesh(const std::vector<float>& vertices,
                                         const std::vector<unsigned int>& indices) {
    auto mesh = std::make_shared<Mesh>();
    if (mesh->Load(vertices, indices)) {
        return mesh;
    }
    return nullptr;
}

void Renderer::RenderMesh(std::shared_ptr<Mesh> mesh) {
    if (mesh) {
        mesh->Render();
        m_stats.drawCalls++;
        m_stats.triangles += mesh->GetTriangleCount();
        m_stats.vertices += mesh->GetVertexCount();
    }
}

void Renderer::RenderMeshInstanced(std::shared_ptr<Mesh> mesh, int instanceCount) {
    if (mesh) {
        mesh->RenderInstanced(instanceCount);
        m_stats.drawCalls++;
        m_stats.triangles += mesh->GetTriangleCount() * instanceCount;
        m_stats.vertices += mesh->GetVertexCount() * instanceCount;
    }
}

std::shared_ptr<Framebuffer> Renderer::CreateFramebuffer(int width, int height) {
    auto framebuffer = std::make_shared<Framebuffer>();
    if (framebuffer->Create(width, height)) {
        return framebuffer;
    }
    return nullptr;
}

void Renderer::BindFramebuffer(std::shared_ptr<Framebuffer> framebuffer) {
    if (framebuffer) {
        framebuffer->Bind();
        m_currentFramebuffer = framebuffer;
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_currentFramebuffer = nullptr;
    }
}

void Renderer::UnbindFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_currentFramebuffer = nullptr;
}

void Renderer::EnableSRGB(bool enable) {
    if (enable) {
        glEnable(GL_FRAMEBUFFER_SRGB);
    } else {
        glDisable(GL_FRAMEBUFFER_SRGB);
    }
}

void Renderer::EnableSeamlessCubemap(bool enable) {
    if (enable) {
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    } else {
        glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    }
}

void Renderer::EnableAnisotropicFiltering(float maxAniso) {
    GLfloat maxAnisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
    maxAniso = std::min(maxAniso, maxAnisotropy);
    
    // This would be applied to textures when they're created
    // For now, just store the value
    m_maxAnisotropy = maxAniso;
}

void Renderer::BeginQuery(const std::string& name) {
    if (m_queries.find(name) == m_queries.end()) {
        GLuint query;
        glGenQueries(1, &query);
        m_queries[name] = query;
    }
    
    glBeginQuery(GL_TIME_ELAPSED, m_queries[name]);
}

void Renderer::EndQuery(const std::string& name) {
    glEndQuery(GL_TIME_ELAPSED);
}

float Renderer::GetQueryTime(const std::string& name) {
    auto it = m_queries.find(name);
    if (it != m_queries.end()) {
        GLuint64 time;
        glGetQueryObjectui64v(it->second, GL_QUERY_RESULT, &time);
        return time / 1000000.0f; // Convert to milliseconds
    }
    return 0.0f;
}

void Renderer::ResetStats() {
    m_stats = {};
}

void Renderer::CreateDefaultVAO() {
    glGenVertexArrays(1, &m_defaultVAO);
    glBindVertexArray(m_defaultVAO);
}

void Renderer::SetupDebugOutput() {
    // Enable debug output if available
    if (glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, 
                                 GLsizei length, const GLchar* message, const void* userParam) {
            if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
                std::cerr << "OpenGL Debug: " << message << std::endl;
            }
        }, nullptr);
    }
}

void Renderer::CheckGLError(const std::string& operation) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error in " << operation << ": " << error << std::endl;
    }
}