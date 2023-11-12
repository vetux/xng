<div>
  <img alt="CODE_LINES" src="https://img.shields.io/github/last-commit/vetux/xng" align="left">
  <img alt="LICENSE" src="https://img.shields.io/github/license/vetux/xng" align="left">
</div>

<br>

# xEngine

xEngine is a cross-platform c++ game engine library.

[xng-editor](https://github.com/vetux/xng-editor) - Editor application to simplify and accelerate the game creation
workflow

## Features

- Resource system
  - Automatic Resource Lifetime handling using RAII and reference counting
- Sprite Animation
- Skeletal Animation
- ECS with data oriented architecture
- FrameGraph Renderer 
  - PBR Lighting
  - Shadow Mapping
- 2D Renderer with object-oriented SDL like interface
- Canvas layout engine with reference scaling support
- Text Rendering
- PAK file format with compression, chunking and encryption support

## Planned Features

- Vulkan GPU driver
- Bullet3 physics driver
 
## Drivers

Drivers are implementations of interfaces (For example through 3rd party libraries) that can be optionally compiled into the engine library. If a driver interface implementation type is instantiated in an application the corresponding driver must be defined by the linked engine library by enabling the corresponding cmake option/s in [drivers.cmake](cmake/drivers.cmake) when compiling the engine library.

- Display Manager
    - GLFW
- 3D AudioData
    - OpenAL-Soft
- GPU (Render, Compute, Raytracing)
    - OpenGL
- Shader Compiler
    - GLSLang
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
    - Tested Compiler: GCC 12.2.0 64bit
- Windows
    - Tested Compilers:
      - MSVC 2022 64bit
          - Build warnings are emitted due to the fact that xng uses STL types in dllexport types.
          - Users must ensure that their application is built with the same compiler version as the linked library because
            windows uses multiple STL dll files and allows loading of multiple STL version dlls into the same process. If
            the user links to a different STL version than the one used for building the engine dll there can be problems
            when passing types through the dll interface.
      - ~~MinGW v8.0.0~~ *
- Android
    - ~~Tested Compiler: Android NDK r25c~~ *

*= Cross Compiling is currently not supported because of the shader precompilation stage (Will be solved in the future)

### Build Instructions

Check [BUILDING.md](BUILDING.md) for detailed building instructions.

## Assimp Parser and Blender FBX Export
When exporting rigged meshes from blender to be imported with the assimp parser driver make sure to:
- Set the "Apply Scalings" to fbx unit scale.
- Rotate the mesh and the rig -90 degrees in the X axis and apply the rotation.
- Set the forward and up axis in the fbx exporter settings to Y forward and Z up to match blenders coordinate system.
- Uncheck "Key All Bones" and "Force Start/End Keyframes"
  - "Key All Bones" appears to do nothing at all
  - "Force Start/End Keyframes" creates invalid keyframes inside an animation for bones which are not keyframed in the animation.
- Run "Limit Total" in weight paint mode to ensure that there is no more than 4 bones per vertex.