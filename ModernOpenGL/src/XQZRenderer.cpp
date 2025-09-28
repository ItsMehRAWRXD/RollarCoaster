#include "XQZRenderer.h"
#include "Shader.h"
#include "Mesh.h"
#include <iostream>

XQZRenderer::XQZRenderer() 
    : m_wireframeColor(0.0f, 1.0f, 0.0f, 0.3f)  // Neon green with transparency
    , m_solidColor(1.0f, 1.0f, 1.0f, 1.0f)      // Solid white
    , m_wireframeEnabled(true)
    , m_solidEnabled(true)
    , m_alpha(0.3f)
    , m_initialized(false)
    , m_previousPolygonMode(GL_FILL)
{
}

XQZRenderer::~XQZRenderer() {
    Shutdown();
}

bool XQZRenderer::Initialize() {
    CreateShaders();
    SetupBlending();
    SetupDepthTesting();
    
    m_initialized = true;
    std::cout << "XQZ Renderer initialized successfully!" << std::endl;
    return true;
}

void XQZRenderer::Shutdown() {
    m_wireframeShader.reset();
    m_solidShader.reset();
    m_initialized = false;
}

void XQZRenderer::BeginXQZPass() {
    if (!m_initialized) return;
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Store previous polygon mode
    glGetIntegerv(GL_POLYGON_MODE, (GLint*)&m_previousPolygonMode);
}

void XQZRenderer::EndXQZPass() {
    if (!m_initialized) return;
    
    // Restore previous polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, m_previousPolygonMode);
    
    // Disable blending
    glDisable(GL_BLEND);
}

void XQZRenderer::RenderWireframe(std::shared_ptr<Mesh> mesh, const glm::mat4& model) {
    if (!m_initialized || !m_wireframeEnabled || !mesh || !m_wireframeShader) return;
    
    // Set wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    // Disable depth testing for X-ray effect
    glDisable(GL_DEPTH_TEST);
    
    // Use wireframe shader
    m_wireframeShader->Use();
    
    // Set uniforms
    m_wireframeShader->SetMat4("uModel", model);
    m_wireframeShader->SetVec4("uColor", m_wireframeColor);
    
    // Render mesh
    mesh->Render();
    
    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}

void XQZRenderer::RenderSolid(std::shared_ptr<Mesh> mesh, const glm::mat4& model) {
    if (!m_initialized || !m_solidEnabled || !mesh || !m_solidShader) return;
    
    // Set solid mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Enable depth testing for solid objects
    glEnable(GL_DEPTH_TEST);
    
    // Use solid shader
    m_solidShader->Use();
    
    // Set uniforms
    m_solidShader->SetMat4("uModel", model);
    m_solidShader->SetVec4("uColor", m_solidColor);
    
    // Render mesh
    mesh->Render();
}

void XQZRenderer::SetWireframeColor(const glm::vec4& color) {
    m_wireframeColor = color;
}

void XQZRenderer::SetSolidColor(const glm::vec4& color) {
    m_solidColor = color;
}

void XQZRenderer::SetWireframeEnabled(bool enabled) {
    m_wireframeEnabled = enabled;
}

void XQZRenderer::SetSolidEnabled(bool enabled) {
    m_solidEnabled = enabled;
}

void XQZRenderer::SetAlpha(float alpha) {
    m_alpha = alpha;
    m_wireframeColor.a = alpha;
}

void XQZRenderer::CreateShaders() {
    // Wireframe shader
    std::string wireframeVertex = R"(
#version 450 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

void main() {
    vec4 worldPos = uModel * vec4(inPos, 1.0);
    gl_Position = uProj * uView * worldPos;
}
)";

    std::string wireframeFragment = R"(
#version 450 core
out vec4 FragColor;

uniform vec4 uColor;

void main() {
    FragColor = uColor;
}
)";

    m_wireframeShader = std::make_shared<Shader>();
    m_wireframeShader->Load(wireframeVertex, wireframeFragment);

    // Solid shader
    std::string solidVertex = R"(
#version 450 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out VS_OUT {
    vec3 worldPos;
    vec3 normal;
} vs_out;

void main() {
    vec4 worldPos = uModel * vec4(inPos, 1.0);
    vs_out.worldPos = worldPos.xyz;
    vs_out.normal = mat3(uModel) * inNormal;
    
    gl_Position = uProj * uView * worldPos;
}
)";

    std::string solidFragment = R"(
#version 450 core
in VS_OUT {
    vec3 worldPos;
    vec3 normal;
} fs_in;

out vec4 FragColor;

uniform vec4 uColor;
uniform vec3 uLightPos;
uniform vec3 uLightColor;

void main() {
    vec3 N = normalize(fs_in.normal);
    vec3 L = normalize(uLightPos - fs_in.worldPos);
    float diff = max(dot(N, L), 0.0);
    
    vec3 lighting = uLightColor * diff;
    FragColor = vec4(uColor.rgb * lighting, uColor.a);
}
)";

    m_solidShader = std::make_shared<Shader>();
    m_solidShader->Load(solidVertex, solidFragment);
}

void XQZRenderer::SetupBlending() {
    // Blending is enabled/disabled in BeginXQZPass/EndXQZPass
}

void XQZRenderer::SetupDepthTesting() {
    // Depth testing is controlled per render call
}