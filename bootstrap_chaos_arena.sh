#!/usr/bin/env bash
set -euo pipefail

: "${GH_USER:?Set GH_USER}"; : "${GH_TOKEN:?Set GH_TOKEN}"; : "${REPO_NAME:?Set REPO_NAME}"

echo "🚀 Bootstrapping Chaos Arena scaffold..."

# Create repo (private by default; flip to true for public)
echo "📦 Creating GitHub repository..."
curl -s -H "Authorization: token $GH_TOKEN" \
  -H "Accept: application/vnd.github+json" \
  https://api.github.com/user/repos \
  -d "{\"name\":\"$REPO_NAME\",\"private\":true,\"auto_init\":false}" >/dev/null || true

mkdir -p "$REPO_NAME"/{src,include,.github/workflows}
cd "$REPO_NAME"

echo "📁 Creating project structure..."

# .gitignore
cat > .gitignore << 'EOF'
build/
cmake-build-*/
.DS_Store
.vscode/
.idea/
*.log
EOF

# CMake
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.16)
project(ChaosArena LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
add_library(engine INTERFACE)
target_include_directories(engine INTERFACE include)
add_executable(chaos src/main.cpp)
target_link_libraries(chaos PRIVATE engine)
if (MSVC)
  target_compile_options(chaos PRIVATE /W4)
else()
  target_compile_options(chaos PRIVATE -Wall -Wextra -Wpedantic)
endif()
EOF

# Minimal engine scaffold header (safe, dev/QA only helpers gated by DEV_BUILD)
cat > include/engine_scaffold.h << 'EOF'
#pragma once
#include <array>
#include <atomic>
#include <vector>
#include <cstdint>

// -------- math --------
struct Vec3{float x=0,y=0,z=0;};
struct Rot {float yaw=0,pitch=0,roll=0;};

// -------- compile gates --------
#if defined(DEV_BUILD)
  #define DEV_ONLY(x) x
#else
  #define DEV_ONLY(x)
#endif

// -------- generic ring buffer --------
template<size_t N, typename T>
struct Ring {
  std::array<T,N> buf{}; std::atomic<size_t> head{0};
  void push(const T& v){ buf[head.fetch_add(1,std::memory_order_relaxed)%N] = v; }
  size_t size() const { size_t h=head.load(std::memory_order_relaxed); return h<N? h:N; }
  T get(size_t i) const { size_t h=head.load(); size_t s=(h>=N? h%N:0); return buf[(s+i)%N]; }
};

// -------- core data --------
struct PlayerState {
  uint32_t id=0; Vec3 pos{}; Rot view{}; int hp=100;
};

struct TelemetryRow { uint64_t t=0; uint32_t evt=0; uint32_t a=0,b=0; float x=0,y=0,z=0; };
using TelemetryRing = Ring<1024, TelemetryRow>;

// -------- dev-only helpers --------
struct DevOverlay {
  DEV_ONLY(void drawCrosshair() const {/* hook your renderer; keep it batched */})
  DEV_ONLY(void drawAABB(const Vec3&, const Vec3&) const {/* instanced boxes */})
};

struct DebugRadar {
  bool enabled=false; float range=120.f;
  DEV_ONLY(void drawMini(const PlayerState& self, const std::vector<PlayerState>& ents) const {
    if(!enabled) return; (void)self; (void)ents; /* draw minimal QA blips */ })
};

struct EvidenceSnap {
  DEV_ONLY(static void captureAngles(uint32_t pid, const Rot& v, TelemetryRing& ring, uint64_t now) {
    ring.push(TelemetryRow{now, /*evt*/1, pid, 0, v.yaw, v.pitch, v.roll});
  })
};

// -------- tiny dev panel --------
struct DevPanel {
  bool active=true, showRadar=false, showOverlay=true, sampleEvidence=false;
  DEV_ONLY(void onTick(PlayerState& you, TelemetryRing& tel, uint64_t now){
    if(sampleEvidence){ EvidenceSnap::captureAngles(you.id, you.view, tel, now); sampleEvidence=false; }
  })
};

// -------- root scaffold --------
struct GameScaffold {
  PlayerState you{};
  std::vector<PlayerState> others;
  TelemetryRing telemetry;
  DEV_ONLY(DevOverlay overlay;)
  DEV_ONLY(DebugRadar radar;)
  DEV_ONLY(DevPanel panel;)

  void init(){
    DEV_ONLY(radar.enabled = panel.showRadar;)
  }
  void tick(uint64_t now){
    DEV_ONLY(panel.onTick(you, telemetry, now);)
    DEV_ONLY(if(panel.showOverlay) overlay.drawCrosshair();)
    DEV_ONLY(if(panel.showRadar)   radar.drawMini(you, others);)
  }
};
EOF

# main.cpp
cat > src/main.cpp << 'EOF'
#include <iostream>
#include "engine_scaffold.h"

int main(){
  GameScaffold game;
  game.init();
  // Simulate one dev snapshot
  game.panel.sampleEvidence = true;
  game.you.id = 1;
  game.you.view = Rot{90.f, 0.f, 0.f};
  game.tick(/*now*/123456789);
  std::cout << "Scaffold OK. Telemetry size=" << game.telemetry.size() << "\n";
  return 0;
}
EOF

# CI: build on Linux & Windows
cat > .github/workflows/ci.yml << 'EOF'
name: ci
on: [push, pull_request]
jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix: { os: [ubuntu-latest, windows-latest] }
    steps:
      - uses: actions/checkout@v4
      - name: Configure
        run: cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
      - name: Build
        run: cmake --build build --config Release --parallel
      - name: Run (Linux only)
        if: runner.os == 'Linux'
        run: ./build/chaos || true
EOF

echo "🔧 Setting up git repository..."

# git init, commit, push
git init
git branch -m main
git add .
git -c user.email="bootstrap@local" -c user.name="$GH_USER" commit -m "feat: initial scaffold (engine, ci, cmake)"
git remote add origin "https://$GH_USER:$GH_TOKEN@github.com/$GH_USER/$REPO_NAME.git"
git push -u origin main

echo "✅ Done! Repo: https://github.com/$GH_USER/$REPO_NAME"
echo ""
echo "🔨 To build locally:"
echo "  cd $REPO_NAME"
echo "  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release"
echo "  cmake --build build --config Release --parallel"
echo "  ./build/chaos"