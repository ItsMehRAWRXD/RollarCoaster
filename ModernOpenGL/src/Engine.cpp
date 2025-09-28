#include "Engine.h"
#include "FrameGraph.h"
#include "Renderer.h"
#include "Camera.h"
#include "AssetLoader.h"
#include <iostream>
#include <chrono>

Engine::Engine() 
    : m_window(nullptr)
    , m_width(1920)
    , m_height(1080)
    , m_title("Modern OpenGL Engine")
    , m_lastFrameTime(0.0)
    , m_deltaTime(0.0f)
    , m_fps(0.0f)
    , m_frameCount(0)
    , m_fpsTimer(0.0)
    , m_running(false)
    , m_initialized(false)
{
}

Engine::~Engine() {
    Shutdown();
}

bool Engine::Initialize(int width, int height, const std::string& title) {
    m_width = width;
    m_height = height;
    m_title = title;

    // Set GLFW error callback
    glfwSetErrorCallback(ErrorCallback);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Configure OpenGL context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

    // Create window
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetWindowUserPointer(m_window, this);

    // Set callbacks
    glfwSetFramebufferSizeCallback(m_window, ResizeCallback);
    glfwSetKeyCallback(m_window, KeyCallback);
    glfwSetCursorPosCallback(m_window, MouseCallback);
    glfwSetScrollCallback(m_window, ScrollCallback);

    // Enable vsync
    glfwSwapInterval(1);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    // Check OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Enable modern OpenGL features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Enable sRGB framebuffer for gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    // Enable seamless cubemap sampling
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Initialize core systems
    m_renderer = std::make_shared<Renderer>();
    if (!m_renderer->Initialize(m_width, m_height)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    m_camera = std::make_shared<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));
    m_assetLoader = std::make_shared<AssetLoader>();
    
    m_frameGraph = std::make_shared<FrameGraph>();
    if (!m_frameGraph->Initialize(m_renderer)) {
        std::cerr << "Failed to initialize frame graph" << std::endl;
        return false;
    }

    m_initialized = true;
    m_running = true;
    
    std::cout << "Engine initialized successfully!" << std::endl;
    return true;
}

void Engine::Run() {
    if (!m_initialized) {
        std::cerr << "Engine not initialized!" << std::endl;
        return;
    }

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (m_running && !glfwWindowShouldClose(m_window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        m_deltaTime = deltaTime;
        UpdateFrameTiming();

        HandleInput();
        Update();
        Render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Engine::Shutdown() {
    if (m_frameGraph) {
        m_frameGraph->Shutdown();
    }
    
    if (m_renderer) {
        m_renderer->Shutdown();
    }

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    
    glfwTerminate();
    m_initialized = false;
}

void Engine::Update() {
    // Update camera
    if (m_camera) {
        m_camera->Update(m_deltaTime);
    }

    // Update frame graph
    if (m_frameGraph) {
        m_frameGraph->Update(m_deltaTime);
    }
}

void Engine::Render() {
    if (m_frameGraph) {
        m_frameGraph->Execute();
    }
}

void Engine::HandleInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        m_running = false;
    }
}

void Engine::UpdateFrameTiming() {
    m_frameCount++;
    m_fpsTimer += m_deltaTime;
    
    if (m_fpsTimer >= 1.0) {
        m_fps = static_cast<float>(m_frameCount) / m_fpsTimer;
        m_frameCount = 0;
        m_fpsTimer = 0.0;
        
        // Update window title with FPS
        std::string title = m_title + " - FPS: " + std::to_string(static_cast<int>(m_fps));
        glfwSetWindowTitle(m_window, title.c_str());
    }
}

// Static callbacks
void Engine::ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void Engine::ResizeCallback(GLFWwindow* window, int width, int height) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine) {
        engine->m_width = width;
        engine->m_height = height;
        glViewport(0, 0, width, height);
        
        if (engine->m_camera) {
            engine->m_camera->SetAspectRatio(static_cast<float>(width) / height);
        }
        
        if (engine->m_renderer) {
            engine->m_renderer->Resize(width, height);
        }
        
        if (engine->m_frameGraph) {
            engine->m_frameGraph->Resize(width, height);
        }
    }
}

void Engine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine && engine->m_camera) {
        engine->m_camera->HandleKeyInput(key, action);
    }
}

void Engine::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine && engine->m_camera) {
        engine->m_camera->HandleMouseInput(xpos, ypos);
    }
}

void Engine::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (engine && engine->m_camera) {
        engine->m_camera->HandleScrollInput(xoffset, yoffset);
    }
}