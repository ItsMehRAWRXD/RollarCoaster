#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <glm/glm.hpp>

// Forward declarations
class Renderer;
class RenderPass;
class Texture;
class Framebuffer;

// Render pass types
enum class PassType {
    GBuffer,
    SSAO,
    Shadows,
    Lighting,
    Transparent,
    PostFX,
    UI
};

// Resource types for the frame graph
enum class ResourceType {
    Albedo,
    Normal,
    Material,
    Depth,
    ShadowMap,
    AO,
    HDRColor,
    LDRColor,
    Backbuffer
};

// Frame graph node representing a render pass
struct FrameGraphNode {
    std::string name;
    PassType type;
    std::vector<ResourceType> reads;
    std::vector<ResourceType> writes;
    std::function<void()> execute;
    bool enabled = true;
    int priority = 0; // Lower numbers execute first
};

class FrameGraph {
public:
    FrameGraph();
    ~FrameGraph();

    bool Initialize(std::shared_ptr<Renderer> renderer);
    void Shutdown();
    void Update(float deltaTime);
    void Execute();
    void Resize(int width, int height);

    // Frame graph building
    FrameGraphNode& AddPass(const std::string& name, PassType type, 
                           const std::vector<ResourceType>& reads,
                           const std::vector<ResourceType>& writes,
                           std::function<void()> execute);
    
    void Compile();
    void SetPassEnabled(const std::string& name, bool enabled);
    void SetPassPriority(const std::string& name, int priority);

    // Resource management
    std::shared_ptr<Texture> GetResource(ResourceType type) const;
    void SetResource(ResourceType type, std::shared_ptr<Texture> texture);

    // Getters
    std::shared_ptr<Renderer> GetRenderer() const { return m_renderer; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

private:
    void CreateDefaultPasses();
    void SortPasses();
    void CreateResources();
    void DestroyResources();

    std::shared_ptr<Renderer> m_renderer;
    std::vector<FrameGraphNode> m_passes;
    std::unordered_map<ResourceType, std::shared_ptr<Texture>> m_resources;
    std::unordered_map<std::string, size_t> m_passIndices;
    
    int m_width, m_height;
    bool m_compiled;
    bool m_initialized;
};