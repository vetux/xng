<div>


<div align="right">
  <img alt="CODE_LINES" src="https://img.shields.io/tokei/lines/github/vetux/xng" align="left">
  <img alt="LICENSE" src="https://img.shields.io/github/license/vetux/xng" align="right">
  <img alt="LAST_COMMIT" src="https://img.shields.io/github/last-commit/vetux/canora" align="right">
</div>

<br>

<div align="center">
  <img src="https://github.com/xenotux/canora/blob/master/docs/mainIcon.png" alt="MainIcon" width="300" height="300"/>
</div>

</div>

# xEngine

xEngine is a cross-platform c++ game engine library. 

[xng-tools](https://github.com/vetux/xng-tools) provides an editor application and various tools to simplify and accelerate the game creation workflow

## Features
- Drivers
  - Display Manager
    - GLFW
  - 3D Audio
    - OpenAL-Soft
  - GPU (Render, Compute, Raytracing)
    - OpenGL
  - Shader Compiler
    - ShaderC 
  - Font Rendering
    - FreeType
  - Physics Simulation
    - Box2D
  - Resource Parsers
    - AssImp
    - libSndFile
- ECS with data oriented architecture
- Text Rendering
- 2D Renderer
- Sprite Animation
- PAK file format

## Planned Features
- Vulkan GPU driver
- Bullet3 physics driver
- Frame Graph Renderer
  - PBR Material support 
- Skeletal animation
- Scripting Abstraction

## Programs
- [xng-tools](https://github.com/vetux/xng-tools) - The editor and tools for creating games with xng
- [Foxtrot](https://github.com/vetux/foxtrot) - A 2D Platformer Game

# Building

- Ensure the external dependencies are provided. ([install_dependencies.sh](install_dependencies.sh]))
- Clone the [imgui](submodules/imgui) submodule
- Load [CMakeLists.txt](CMakeLists.txt) and build the engine target