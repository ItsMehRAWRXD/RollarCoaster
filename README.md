# Chaos Arena Bootstrap 🚀

**Self-bootstrapping C++ game engine scaffold** - Get a working repo with dev tools, telemetry, and CI in 2 minutes.

## What You Get

✅ **GitHub Repository** - Private repo with your scaffold  
✅ **C++ Engine** - Dev tools, telemetry ring, player state  
✅ **CMake Build** - Cross-platform build system  
✅ **GitHub Actions** - CI on Linux & Windows  
✅ **Dev Panel** - Toggle radar, overlay, evidence capture  

## Quick Start (2 minutes)

### 1. Get GitHub Token
1. Go to https://github.com/settings/tokens
2. Click "Generate new token (classic)"
3. Select "repo" scope
4. Copy the token (starts with `ghp_`)

### 2. Set Environment Variables
```bash
export GH_USER="your_github_username"
export GH_TOKEN="ghp_XXXXXXXXXXXXXXXXXXXXXXXX"   # your token
export REPO_NAME="chaos-arena-scaffold"
```

### 3. Run Bootstrap
```bash
./bootstrap_chaos_arena.sh
```

### 4. Build & Test
```bash
cd chaos-arena-scaffold
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
./build/chaos
```

## Files Created

- `include/engine_scaffold.h` - Core engine with dev tools
- `src/main.cpp` - Test harness  
- `CMakeLists.txt` - Build configuration
- `.github/workflows/ci.yml` - GitHub Actions CI
- `.gitignore` - Standard C++ ignores

## Engine Features

### Dev Tools (DEV_BUILD gated)
- **DevPanel**: Toggle radar, overlay, evidence capture
- **DebugRadar**: Mini-map with player blips
- **DevOverlay**: Crosshair, AABB visualization
- **EvidenceSnap**: Telemetry capture for QA

### Core Systems
- **PlayerState**: ID, position, rotation, health
- **TelemetryRing**: Lock-free ring buffer for events
- **GameScaffold**: Root container with tick loop

### Math Types
- **Vec3**: 3D position/velocity
- **Rot**: Yaw/pitch/roll rotation

## Next Steps

The scaffold is ready for:
- **Movement sandbox**: Add movement/ module for skiing/wall-run experiments
- **Server authority**: Create server/ with hit-validation functions  
- **Dev toggles**: Wire DevPanel into your UI layer

All dev tools are gated behind `DEV_BUILD` so they won't ship in release builds.

## Test Locally First

```bash
./test_scaffold.sh
```

This creates a local test without GitHub to verify everything works.

---

**Ready to ship!** 🪢 You've got the rope - now go build your chaos arena.