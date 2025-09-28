#include "Engine.h"
#include "DeferredRenderer.h"
#include "XQZRenderer.h"
#include "MapGenerator.h"
#include "SnapshotSystem.h"
#include "Mesh.h"
#include "Camera.h"
#include <iostream>
#include <chrono>

// Demo scene data
struct DemoScene {
    std::shared_ptr<Mesh> cube;
    std::shared_ptr<Mesh> sphere;
    std::shared_ptr<Mesh> plane;
    std::shared_ptr<Mesh> wallMesh;
    
    // Transform matrices
    glm::mat4 cubeModel;
    glm::mat4 sphereModel;
    glm::mat4 planeModel;
    std::vector<glm::mat4> wallModels;
    
    // Animation
    float rotation = 0.0f;
    float time = 0.0f;
    
    // Map data
    Map currentMap;
    int mapSeed = 1337;
};

// Create demo scene
DemoScene CreateDemoScene(std::shared_ptr<Renderer> renderer) {
    DemoScene scene;
    
    // Create cube mesh
    std::vector<float> cubeVertices = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    std::vector<unsigned int> cubeIndices;
    for (unsigned int i = 0; i < 36; i++) {
        cubeIndices.push_back(i);
    }
    
    scene.cube = renderer->CreateMesh(cubeVertices, cubeIndices);
    
    // Create plane mesh (ground)
    std::vector<float> planeVertices = {
        // positions          // normals           // texture coords
        -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
         10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        -10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f
    };
    
    std::vector<unsigned int> planeIndices = { 0, 1, 2, 3, 4, 5 };
    scene.plane = renderer->CreateMesh(planeVertices, planeIndices);
    
    // Create wall mesh (simple cube)
    std::vector<float> wallVertices = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    
    std::vector<unsigned int> wallIndices;
    for (unsigned int i = 0; i < 36; i++) {
        wallIndices.push_back(i);
    }
    
    scene.wallMesh = renderer->CreateMesh(wallVertices, wallIndices);
    
    // Initialize transform matrices
    scene.cubeModel = glm::mat4(1.0f);
    scene.sphereModel = glm::mat4(1.0f);
    scene.planeModel = glm::mat4(1.0f);
    
    return scene;
}

int main() {
    std::cout << "Modern OpenGL Engine Demo" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // Create engine
    Engine engine;
    
    // Initialize engine
    if (!engine.Initialize(1920, 1080, "Modern OpenGL Engine")) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }
    
    // Get renderer
    auto renderer = engine.GetRenderer();
    if (!renderer) {
        std::cerr << "Failed to get renderer!" << std::endl;
        return -1;
    }
    
    // Create deferred renderer
    auto deferredRenderer = std::make_shared<DeferredRenderer>();
    if (!deferredRenderer->Initialize(1920, 1080)) {
        std::cerr << "Failed to initialize deferred renderer!" << std::endl;
        return -1;
    }
    
    // Create XQZ renderer
    auto xqzRenderer = std::make_shared<XQZRenderer>();
    if (!xqzRenderer->Initialize()) {
        std::cerr << "Failed to initialize XQZ renderer!" << std::endl;
        return -1;
    }
    
    // Generate procedural map
    std::cout << "Generating procedural map..." << std::endl;
    scene.currentMap = MapGenerator::GenerateArena(scene.mapSeed, 50);
    std::cout << "Generated map with " << scene.currentMap.walls.size() << " walls and " 
              << scene.currentMap.spawns.size() << " spawns" << std::endl;
    
    // Create wall models from map
    scene.wallModels.clear();
    for (const auto& wall : scene.currentMap.walls) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, wall.position);
        model = glm::rotate(model, glm::radians(wall.rotation.y), glm::vec3(0, 1, 0));
        model = glm::scale(model, wall.size);
        scene.wallModels.push_back(model);
    }
    
    // Create network prediction systems
    SnapshotBuffer snapshotBuffer;
    NetworkPrediction networkPrediction;
    PingCompensation pingCompensation;
    MuzzleCorrection muzzleCorrection;
    
    // Set up muzzle offset
    muzzleCorrection.SetMuzzleOffset(glm::vec3(0.3f, 0.0f, 0.0f));
    
    // Create demo scene
    DemoScene scene = CreateDemoScene(renderer);
    
    // Create camera
    auto camera = engine.GetCamera();
    if (!camera) {
        std::cerr << "Failed to get camera!" << std::endl;
        return -1;
    }
    
    // Set camera position
    camera->SetPosition(glm::vec3(0.0f, 2.0f, 5.0f));
    camera->SetTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Create shaders
    auto gbufferShader = renderer->CreateShader(
        Shader::GetGBufferVertexShader(),
        Shader::GetGBufferFragmentShader()
    );
    
    auto lightingShader = renderer->CreateShader(
        Shader::GetHDRVertexShader(),
        Shader::GetLightingFragmentShader()
    );
    
    auto toneMapShader = renderer->CreateShader(
        Shader::GetHDRVertexShader(),
        Shader::GetACESToneMapShader()
    );
    
    if (!gbufferShader || !lightingShader || !toneMapShader) {
        std::cerr << "Failed to create shaders!" << std::endl;
        return -1;
    }
    
    // Create default textures
    auto defaultAlbedo = renderer->CreateTexture(1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, "DefaultAlbedo");
    auto defaultNormal = renderer->CreateTexture(1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, "DefaultNormal");
    auto defaultMaterial = renderer->CreateTexture(1, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, "DefaultMaterial");
    
    // Set default texture data
    if (defaultAlbedo) {
        unsigned char albedoData[] = { 128, 128, 128, 255 }; // Gray
        defaultAlbedo->SetData(albedoData);
    }
    
    if (defaultNormal) {
        unsigned char normalData[] = { 128, 128, 255, 255 }; // Normal pointing up
        defaultNormal->SetData(normalData);
    }
    
    if (defaultMaterial) {
        unsigned char materialData[] = { 0, 128, 255, 255 }; // Metallic=0, Roughness=0.5, AO=1
        defaultMaterial->SetData(materialData);
    }
    
    // Main loop
    std::cout << "Starting main loop..." << std::endl;
    
    while (engine.GetWindow() && !glfwWindowShouldClose(engine.GetWindow())) {
        float deltaTime = engine.GetDeltaTime();
        scene.time += deltaTime;
        scene.rotation += deltaTime * 50.0f; // 50 degrees per second
        
        // Update scene transforms
        scene.cubeModel = glm::mat4(1.0f);
        scene.cubeModel = glm::translate(scene.cubeModel, glm::vec3(0.0f, 1.0f, 0.0f));
        scene.cubeModel = glm::rotate(scene.cubeModel, glm::radians(scene.rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        scene.cubeModel = glm::scale(scene.cubeModel, glm::vec3(1.0f, 1.0f, 1.0f));
        
        // Update camera
        camera->Update(deltaTime);
        
        // Get camera matrices
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 proj = camera->GetProjectionMatrix();
        glm::mat4 viewProj = proj * view;
        glm::vec3 eyePos = camera->GetPosition();
        
        // Begin frame
        renderer->BeginFrame();
        
        // XQZ Wireframe Pass - Render map as translucent neon walls
        xqzRenderer->BeginXQZPass();
        
        // Set camera uniforms for XQZ shader
        auto wireframeShader = xqzRenderer->GetWireframeShader();
        if (wireframeShader) {
            wireframeShader->Use();
            wireframeShader->SetMat4("uView", view);
            wireframeShader->SetMat4("uProj", proj);
            wireframeShader->SetVec4("uColor", glm::vec4(0.0f, 1.0f, 0.0f, 0.3f)); // Neon green
        }
        
        // Render all walls as wireframe
        for (size_t i = 0; i < scene.wallModels.size(); i++) {
            xqzRenderer->RenderWireframe(scene.wallMesh, scene.wallModels[i]);
        }
        
        xqzRenderer->EndXQZPass();
        
        // G-buffer pass for solid objects
        deferredRenderer->BeginGBuffer();
        gbufferShader->Use();
        
        // Set camera uniforms
        gbufferShader->SetMat4("uView", view);
        gbufferShader->SetMat4("uProj", proj);
        gbufferShader->SetMat4("uViewProj", viewProj);
        gbufferShader->SetVec3("uEyePos", eyePos);
        
        // Set material uniforms
        gbufferShader->SetVec3("uAlbedo", glm::vec3(0.8f, 0.2f, 0.2f)); // Red
        gbufferShader->SetFloat("uMetallic", 0.0f);
        gbufferShader->SetFloat("uRoughness", 0.5f);
        gbufferShader->SetFloat("uAO", 1.0f);
        
        // Bind default textures
        renderer->BindTexture(defaultAlbedo, 0);
        renderer->BindTexture(defaultNormal, 1);
        renderer->BindTexture(defaultMaterial, 2);
        renderer->BindTexture(defaultMaterial, 3);
        renderer->BindTexture(defaultMaterial, 4);
        
        gbufferShader->SetInt("uAlbedoMap", 0);
        gbufferShader->SetInt("uNormalMap", 1);
        gbufferShader->SetInt("uMetallicMap", 2);
        gbufferShader->SetInt("uRoughnessMap", 3);
        gbufferShader->SetInt("uAOMap", 4);
        
        // Render solid objects to G-buffer (player cube, pickups, etc.)
        deferredRenderer->RenderGBuffer(scene.cube, scene.cubeModel);
        deferredRenderer->RenderGBuffer(scene.plane, scene.planeModel);
        
        deferredRenderer->EndGBuffer();
        
        // Lighting pass
        deferredRenderer->BeginLighting();
        lightingShader->Use();
        
        // Set camera uniforms
        lightingShader->SetMat4("uView", view);
        lightingShader->SetMat4("uProj", proj);
        lightingShader->SetMat4("uViewProj", viewProj);
        lightingShader->SetVec3("uEyePos", eyePos);
        
        // Set lighting uniforms
        lightingShader->SetInt("uLightCount", 1);
        lightingShader->SetVec3("uLights[0].position", glm::vec3(2.0f, 4.0f, 2.0f));
        lightingShader->SetVec3("uLights[0].color", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader->SetFloat("uLights[0].intensity", 1.0f);
        lightingShader->SetFloat("uLights[0].radius", 10.0f);
        
        deferredRenderer->RenderLighting();
        deferredRenderer->EndLighting();
        
        // Post-FX pass
        deferredRenderer->BeginPostFX();
        toneMapShader->Use();
        
        // Bind HDR texture
        renderer->BindTexture(deferredRenderer->GetHDRTexture(), 0);
        toneMapShader->SetInt("uHDRTexture", 0);
        toneMapShader->SetFloat("uExposure", 1.0f);
        toneMapShader->SetBool("uUseACES", true);
        
        deferredRenderer->RenderPostFX();
        deferredRenderer->EndPostFX();
        
        // Render to screen
        deferredRenderer->RenderToScreen();
        
        // End frame
        renderer->EndFrame();
        
        // Update window
        glfwSwapBuffers(engine.GetWindow());
        glfwPollEvents();
        
        // Print FPS every second
        static float fpsTimer = 0.0f;
        static int frameCount = 0;
        fpsTimer += deltaTime;
        frameCount++;
        
        if (fpsTimer >= 1.0f) {
            std::cout << "FPS: " << frameCount << " | Frame Time: " << (fpsTimer / frameCount) * 1000.0f << "ms" << std::endl;
            fpsTimer = 0.0f;
            frameCount = 0;
        }
    }
    
    std::cout << "Shutting down..." << std::endl;
    
    // Cleanup
    deferredRenderer->Shutdown();
    engine.Shutdown();
    
    return 0;
}