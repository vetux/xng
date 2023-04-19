<div>
  <img alt="CODE_LINES" src="https://img.shields.io/tokei/lines/github/vetux/xng" align="left">
  <img alt="LICENSE" src="https://img.shields.io/github/license/vetux/xng" align="left">
</div>

<br>

# xEngine

xEngine is a cross-platform c++ game engine library.

[xng-editor](https://github.com/vetux/xng-editor) - Editor application to simplify and accelerate the game creation
workflow

## Features

- ECS with data oriented architecture
- 2D Renderer with multi draw acceleration
- Canvas layout engine with reference scaling support
- Text Rendering
- Sprite Animation
- PAK file format
- Resource system

## Planned Features

- Vulkan GPU driver
- Bullet3 physics driver
- Frame Graph Renderer
    - PBR Material support
- Skeletal animation
 
## Drivers

Drivers are implementations of interfaces (For example through 3rd party libraries) that can be optionally compiled into the engine library. If a driver interface implementation type is instantiated in an application the corresponding driver must be defined by the linked engine library by enabling the corresponding cmake option/s in [drivers.cmake](cmake/drivers.cmake) when compiling the engine library.

- Display Manager
    - GLFW
- 3D Audio
    - OpenAL-Soft
- GPU (Render, Compute, Raytracing)
    - OpenGL
- Shader Compiler
    - ShaderC
- Shader Decompiler
    - SPIRV-Cross  
- Font Rendering
    - FreeType
- Physics Simulation
    - Box2D
- Resource Parsers
    - AssImp
    - libSndFile

## Programs

- [xng-editor](https://github.com/vetux/xng-editor) - The editor and tools for creating games with xng
- [Foxtrot](https://github.com/vetux/foxtrot) - A 2D Platformer Game

## Building

### Supported Platforms

- Linux
    - Tested Compiler: GCC 10.2.1 64bit
- Windows
    - Tested Compiler: MSVC 2022 64bit
        - Build warnings are emitted due to the fact that xng uses STL types in dllexport types.
        - Users must ensure that their application is built with the same compiler version as the linked library because
          windows uses multiple STL dll files and allows loading of multiple STL version dlls into the same process. If
          the user links to a different STL version than the one used for building the engine dll there can be problems
          when passing types through the dll interface.
    - Tested Compiler: MinGW v8.0.0
- Android
    - Tested Compiler: Android NDK r25c

### Build Instructions

Check [BUILDING.md](BUILDING.md) for detailed building instructions.