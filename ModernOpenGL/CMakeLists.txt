cmake_minimum_required(VERSION 3.16)
project(ModernOpenGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)

# Find GLM
find_package(glm REQUIRED)

# Optional: Find Assimp for glTF loading
find_package(assimp QUIET)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Source files
set(SOURCES
    src/main.cpp
    src/Engine.cpp
    src/FrameGraph.cpp
    src/Renderer.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Mesh.cpp
    src/Camera.cpp
    src/Lighting.cpp
    src/PostFX.cpp
    src/AssetLoader.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
)

# Optional: Link Assimp if found
if(assimp_FOUND)
    target_link_libraries(${PROJECT_NAME} assimp::assimp)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_ASSIMP)
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)