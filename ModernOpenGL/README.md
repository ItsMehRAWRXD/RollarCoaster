# Modern OpenGL Engine

A modern OpenGL 4.5+ engine with deferred rendering, PBR materials, HDR tone mapping, and frame prediction systems.

## Features

- **Modern OpenGL 4.5 Core Profile** - No legacy immediate mode
- **Deferred Rendering** - G-buffer with albedo, normal, material channels
- **PBR Materials** - Physically based rendering with metallic-roughness workflow
- **HDR Pipeline** - R11G11B10F framebuffer with ACES tone mapping
- **Frame Prediction** - Client-side interpolation and extrapolation
- **Network Ready** - Snapshot system with ping compensation
- **Performance Optimized** - Multi-draw indirect, bindless textures, compute shaders

## Quick Start

```bash
# Clone the repository
git clone <your-repo-url>
cd ModernOpenGL

# Build with CMake
cmake -S . -B build
cmake --build build

# Run the demo
./build/bin/ModernOpenGL
```

## Architecture

### Core Systems
- **Engine** - Main application loop and system coordination
- **Renderer** - OpenGL state management and resource handling
- **FrameGraph** - Render pass orchestration
- **DeferredRenderer** - G-buffer and lighting pipeline

### Rendering Pipeline
1. **G-buffer Pass** - Render geometry to albedo, normal, material textures
2. **Lighting Pass** - Deferred lighting with PBR BRDF
3. **Post-FX Pass** - Tone mapping, bloom, SSAO
4. **UI Pass** - Render interface elements

### Network Systems
- **SnapshotBuffer** - Client-side interpolation
- **NetworkClient** - WebSocket communication
- **NetworkServer** - Authoritative game state

## Dependencies

- OpenGL 4.5+
- GLFW 3.3+
- GLEW 2.1+
- GLM 0.9.9+
- CMake 3.16+

## Building

The project uses CMake with FetchContent to automatically download dependencies:

```cmake
# Dependencies are automatically fetched
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
```

## Performance Targets

- **60 FPS** on mid-range hardware
- **30+ FPS** on integrated graphics
- **<150ms** interpolation buffer
- **<30 kB/s** network bandwidth per client

## License

MIT License - see LICENSE file for details.