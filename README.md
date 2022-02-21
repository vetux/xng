# xEngine
This is an experimental cross-platform game engine library written in C++.

## Features
- Deferred render pipeline
  - GBuffer multisampling support
  - User defined render pass support
- Audio
  - OpenAL backend 
- Display / Input
  - GLFW backend 
- Graphics
  - Shader cross-compiler abstraction with SPIRV, HLSL and GLSL support
  - OpenGL 4.1 backend
- ECS with aggregated components storage pattern

## Programs
- [xEditor](https://github.com/xenotux/xeditor)
- [xSamples](https://github.com/xenotux/xsamples)

# Building
- Ensure the external dependencies are provided. ([install_dependencies.sh](install_dependencies.sh]))
- Clone the [imgui](submodules/imgui) submodule
- Load [CMakeLists.txt](CMakeLists.txt) and build the engine target