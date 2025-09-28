#include "FrameGraph.h"
#include "Renderer.h"
#include "Texture.h"
#include "Shader.h"
#include <algorithm>
#include <iostream>

FrameGraph::FrameGraph() 
    : m_width(1920)
    , m_height(1080)
    , m_compiled(false)
    , m_initialized(false)
{
}

FrameGraph::~FrameGraph() {
    Shutdown();
}

bool FrameGraph::Initialize(std::shared_ptr<Renderer> renderer) {
    m_renderer = renderer;
    m_width = renderer->GetWidth();
    m_height = renderer->GetHeight();
    
    CreateDefaultPasses();
    CreateResources();
    Compile();
    
    m_initialized = true;
    std::cout << "Frame graph initialized with " << m_passes.size() << " passes" << std::endl;
    return true;
}

void FrameGraph::Shutdown() {
    DestroyResources();
    m_passes.clear();
    m_passIndices.clear();
    m_initialized = false;
}

void FrameGraph::Update(float deltaTime) {
    // Update any dynamic resources or pass parameters
}

void FrameGraph::Execute() {
    if (!m_compiled || !m_initialized) {
        return;
    }

    // Execute passes in priority order
    for (const auto& pass : m_passes) {
        if (pass.enabled && pass.execute) {
            pass.execute();
        }
    }
}

void FrameGraph::Resize(int width, int height) {
    m_width = width;
    m_height = height;
    
    // Recreate resources with new dimensions
    DestroyResources();
    CreateResources();
}

FrameGraphNode& FrameGraph::AddPass(const std::string& name, PassType type, 
                                   const std::vector<ResourceType>& reads,
                                   const std::vector<ResourceType>& writes,
                                   std::function<void()> execute) {
    FrameGraphNode node;
    node.name = name;
    node.type = type;
    node.reads = reads;
    node.writes = writes;
    node.execute = execute;
    
    m_passes.push_back(node);
    m_passIndices[name] = m_passes.size() - 1;
    
    return m_passes.back();
}

void FrameGraph::Compile() {
    SortPasses();
    m_compiled = true;
    std::cout << "Frame graph compiled successfully" << std::endl;
}

void FrameGraph::SetPassEnabled(const std::string& name, bool enabled) {
    auto it = m_passIndices.find(name);
    if (it != m_passIndices.end()) {
        m_passes[it->second].enabled = enabled;
    }
}

void FrameGraph::SetPassPriority(const std::string& name, int priority) {
    auto it = m_passIndices.find(name);
    if (it != m_passIndices.end()) {
        m_passes[it->second].priority = priority;
    }
}

std::shared_ptr<Texture> FrameGraph::GetResource(ResourceType type) const {
    auto it = m_resources.find(type);
    return (it != m_resources.end()) ? it->second : nullptr;
}

void FrameGraph::SetResource(ResourceType type, std::shared_ptr<Texture> texture) {
    m_resources[type] = texture;
}

void FrameGraph::CreateDefaultPasses() {
    // GBuffer pass - renders geometry to G-buffer
    AddPass("GBuffer", PassType::GBuffer, 
            {}, 
            {ResourceType::Albedo, ResourceType::Normal, ResourceType::Material, ResourceType::Depth},
            [this]() {
                // GBuffer rendering implementation
                std::cout << "Executing GBuffer pass" << std::endl;
            });

    // SSAO pass - ambient occlusion
    AddPass("SSAO", PassType::SSAO,
            {ResourceType::Normal, ResourceType::Depth},
            {ResourceType::AO},
            [this]() {
                std::cout << "Executing SSAO pass" << std::endl;
            });

    // Shadow pass - render shadow maps
    AddPass("Shadows", PassType::Shadows,
            {},
            {ResourceType::ShadowMap},
            [this]() {
                std::cout << "Executing Shadows pass" << std::endl;
            });

    // Lighting pass - deferred lighting
    AddPass("Lighting", PassType::Lighting,
            {ResourceType::Albedo, ResourceType::Normal, ResourceType::Material, 
             ResourceType::Depth, ResourceType::ShadowMap, ResourceType::AO},
            {ResourceType::HDRColor},
            [this]() {
                std::cout << "Executing Lighting pass" << std::endl;
            });

    // Transparent pass - forward rendering for transparent objects
    AddPass("Transparent", PassType::Transparent,
            {ResourceType::HDRColor, ResourceType::Depth},
            {ResourceType::HDRColor},
            [this]() {
                std::cout << "Executing Transparent pass" << std::endl;
            });

    // PostFX pass - tone mapping, bloom, etc.
    AddPass("PostFX", PassType::PostFX,
            {ResourceType::HDRColor},
            {ResourceType::LDRColor},
            [this]() {
                std::cout << "Executing PostFX pass" << std::endl;
            });

    // UI pass - render UI elements
    AddPass("UI", PassType::UI,
            {ResourceType::LDRColor},
            {ResourceType::Backbuffer},
            [this]() {
                std::cout << "Executing UI pass" << std::endl;
            });

    // Set priorities (lower numbers execute first)
    SetPassPriority("GBuffer", 0);
    SetPassPriority("SSAO", 1);
    SetPassPriority("Shadows", 1);
    SetPassPriority("Lighting", 2);
    SetPassPriority("Transparent", 3);
    SetPassPriority("PostFX", 4);
    SetPassPriority("UI", 5);
}

void FrameGraph::SortPasses() {
    std::sort(m_passes.begin(), m_passes.end(), 
              [](const FrameGraphNode& a, const FrameGraphNode& b) {
                  return a.priority < b.priority;
              });
}

void FrameGraph::CreateResources() {
    if (!m_renderer) return;

    // Create G-buffer textures
    m_resources[ResourceType::Albedo] = m_renderer->CreateTexture(
        m_width, m_height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, "Albedo"
    );
    
    m_resources[ResourceType::Normal] = m_renderer->CreateTexture(
        m_width, m_height, GL_RGBA16F, GL_RGBA, GL_FLOAT, "Normal"
    );
    
    m_resources[ResourceType::Material] = m_renderer->CreateTexture(
        m_width, m_height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, "Material"
    );
    
    m_resources[ResourceType::Depth] = m_renderer->CreateTexture(
        m_width, m_height, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, "Depth"
    );

    // Create HDR color buffer
    m_resources[ResourceType::HDRColor] = m_renderer->CreateTexture(
        m_width, m_height, GL_R11F_G11F_B10F, GL_RGB, GL_FLOAT, "HDRColor"
    );

    // Create LDR color buffer
    m_resources[ResourceType::LDRColor] = m_renderer->CreateTexture(
        m_width, m_height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, "LDRColor"
    );

    // Create AO buffer
    m_resources[ResourceType::AO] = m_renderer->CreateTexture(
        m_width, m_height, GL_R8, GL_RED, GL_UNSIGNED_BYTE, "AO"
    );

    // Create shadow map
    m_resources[ResourceType::ShadowMap] = m_renderer->CreateTexture(
        2048, 2048, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, "ShadowMap"
    );

    std::cout << "Created " << m_resources.size() << " frame graph resources" << std::endl;
}

void FrameGraph::DestroyResources() {
    m_resources.clear();
}