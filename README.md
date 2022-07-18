# xEngine
This is an experimental cross-platform game engine library written in C++.

## Features
- Interface based Driver Abstractions
  - Display and Input
    - GLFW implementation
  - GPU Abstraction with optional Compute and Raytracing support (WIP)
    - OpenGL 4.2 implementation
  - 3D Audio Playback
    - OpenAL implementation
  - Font Renderer
    - FreeType implementation
  - Physics Simulation
    - Bullet3 implementation (WIP)
    - Box2D implementation (WIP)
  - Resource Parsers
    - AssImp implementation
    - libsndfile implementation
  - SPIRV-Compiler
    - ShaderC implementation
  - SPIRV-Decompiler
    - SPIRV-Cross implementation
  - Crypto
    - CryptoPP implementation
- Frame Graph Renderer (WIP)
  - PBR Materials
  - Phong Shading
  - Shadow Mapping
- 2D Renderer
- Skeletal Animation (WIP)
- Sprite Animation
- PAK file format
- ECS with data oriented architecture

## Programs
- [xEditor](https://github.com/vetux/xeditor)
- [xSamples](https://github.com/vetux/xsamples)
- [GameOfLife](https://github.com/vetux/gameoflife)

# Building
- Ensure the external dependencies are provided. ([install_dependencies.sh](install_dependencies.sh]))
- Clone the [imgui](submodules/imgui) submodule
- Load [CMakeLists.txt](CMakeLists.txt) and build the engine target