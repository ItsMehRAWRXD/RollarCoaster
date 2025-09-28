#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

// Forward declarations
class FrameGraph;
class Renderer;
class Camera;
class AssetLoader;

class Engine {
public:
    Engine();
    ~Engine();

    bool Initialize(int width, int height, const std::string& title);
    void Run();
    void Shutdown();

    // Getters
    GLFWwindow* GetWindow() const { return m_window; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    float GetAspectRatio() const { return static_cast<float>(m_width) / m_height; }
    
    // Frame timing
    float GetDeltaTime() const { return m_deltaTime; }
    float GetFPS() const { return m_fps; }

    // Core systems
    std::shared_ptr<FrameGraph> GetFrameGraph() const { return m_frameGraph; }
    std::shared_ptr<Renderer> GetRenderer() const { return m_renderer; }
    std::shared_ptr<Camera> GetCamera() const { return m_camera; }
    std::shared_ptr<AssetLoader> GetAssetLoader() const { return m_assetLoader; }

private:
    void Update();
    void Render();
    void HandleInput();
    void UpdateFrameTiming();

    // Window and context
    GLFWwindow* m_window;
    int m_width, m_height;
    std::string m_title;

    // Core systems
    std::shared_ptr<FrameGraph> m_frameGraph;
    std::shared_ptr<Renderer> m_renderer;
    std::shared_ptr<Camera> m_camera;
    std::shared_ptr<AssetLoader> m_assetLoader;

    // Frame timing
    double m_lastFrameTime;
    float m_deltaTime;
    float m_fps;
    int m_frameCount;
    double m_fpsTimer;

    // Engine state
    bool m_running;
    bool m_initialized;

    // Static callbacks
    static void ErrorCallback(int error, const char* description);
    static void ResizeCallback(GLFWwindow* window, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};