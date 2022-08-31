# xEngine

xEngine is a cross-platform c++ game engine library.

## Features
- Driver Interfaces
  - Display Manager abstraction
    - GLFW implementation
  - 3D Audio
    - OpenAL-Soft implementation
  - GPU (Render, Compute, Raytracing(WIP))
    - OpenGL implementation 
  - Text Rendering
  - Physics Simulation
- Frame Graph Renderer (WIP)
- 2D Renderer
- Skeletal Animation (WIP)
- Sprite Animation
- PAK file format
- ECS with data oriented architecture

## Programs
- [xEditor](https://github.com/vetux/xeditor) (WIP) - The editor and tools which utilize xng and provide gui for creating games with xng
- [Foxtrot](https://github.com/vetux/foxtrot) - A 2D Platformer Game

# Building

- Ensure the external dependencies are provided. ([install_dependencies.sh](install_dependencies.sh]))
- Clone the [imgui](submodules/imgui) submodule
- Load [CMakeLists.txt](CMakeLists.txt) and build the engine target