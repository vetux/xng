# xEngine
This is an experimental cross-platform game engine library written in C++.

## Features
- Interface based Driver Abstractions
  - Display and Input
    - GLFW implementation
  - Graphics
    - OpenGL 4.2 implementation
  - Audio
    - OpenAL implementation
- Frame Graph Renderer
- 2D Renderer
- Text Rendering
- PAK file format
- ECS with data oriented architecture
- Shader Cross-Compiler Abstraction with SPIRV, HLSL and GLSL support


## Programs
- [xEditor](https://github.com/vetux/xeditor)
- [xSamples](https://github.com/vetux/xsamples)
- [GameOfLife](https://github.com/vetux/gameoflife)

# Building
- Ensure the external dependencies are provided. ([install_dependencies.sh](install_dependencies.sh]))
- Clone the [imgui](submodules/imgui) submodule
- Load [CMakeLists.txt](CMakeLists.txt) and build the engine target