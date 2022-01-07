# Engine
This is a cross-platform game engine library written in C++.

## Features
- Platform abstraction
    - Audio
      - OpenAL backend 
    - Display / Input
      - GLFW backend 
    - Graphics
      - Shader cross-compiler which supports SPIRV, HLSL and GLSL
      - OpenGL 4.6 backend
      - Qt-OpenGL backend for integrating the engine in a Qt application with a QOpenGLWidget
- ImGui Compatibility functions
- Qt Compatibility widget
- Deferred render pipeline
- ECS with aggregated components storage pattern

# Building
- Ensure the external dependencies are provided. ([install_dependencies.sh](install_dependencies.sh]))
- Clone the imgui [submodule](submodules/imgui)
- Load [CMakeLists.txt](CMakeLists.txt) and build the engine target