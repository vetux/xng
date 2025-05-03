<div>
  <img alt="CODE_LINES" src="https://img.shields.io/github/last-commit/vetux/xng" align="left">
  <img alt="LICENSE" src="https://img.shields.io/github/license/vetux/xng" align="left">
</div>

<br>

# xEngine

xEngine is a cross-platform c++ game engine library. 

The engine is designed as a standalone modular toolkit in which different modules can be used independently. 

[xng-editor](https://github.com/vetux/xng-editor) - Editor application to simplify and accelerate the game creation
workflow

## Features

- 3D Frame Graph Renderer
    - Physically Based Lighting
    - Shadow Mapping
- 2D Renderer with object-oriented SDL like interface
- Resource system
  - Uri Resource addressing
  - Multithreaded Resource streaming
  - Automatic Resource Lifetime handling
- ECS using data oriented architecture
- Text Rendering
- Sprite Animation
- Skeletal Animation
- Canvas layout engine
- PAK file format with compression, chunking and encryption support
- Cross Plattform (Linux, Windows and Android)

## Planned Features
- Vulkan GPU driver
- Frame Graph Renderer rewrite to utilize hardware features of Vulkan
- Image Based Environment Lighting for the PBR Model
 
## Screenshots
![](https://raw.githubusercontent.com/vetux/xng-assets/refs/heads/master/screenshots/ScreenshotFramegraph.jpg)

## Drivers

Drivers are implementations of interfaces in the xEngine API (For example through 3rd party libraries) that can be optionally compiled into the engine library. If a driver interface implementation type is instantiated in an application the corresponding driver must be defined by the linked engine library by enabling the corresponding cmake option/s in [drivers.cmake](cmake/drivers.cmake) when compiling the engine library.

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
    - Bullet3
- Resource Importers
    - AssImp
    - libSndFile

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
      - MinGW-w64 11
- Android
    - Tested Compiler: Android NDK r25c

### Build Instructions

Check [BUILDING.md](BUILDING.md) for detailed building instructions.

## Notes
### Assimp Importer and Blender FBX Export
When exporting rigged meshes from blender to be imported with the assimp importer driver make sure to:
- Set the "Apply Scalings" to fbx unit scale.
- Rotate the mesh and the rig -90 degrees in the X axis and apply the rotation.
- Set the forward and up axis in the fbx exporter settings to Y forward and Z up to match blenders coordinate system.
- Uncheck "Key All Bones" and "Force Start/End Keyframes"
  - "Key All Bones" appears to do nothing at all
  - "Force Start/End Keyframes" creates invalid keyframes inside an animation for bones which are not keyframed in the animation.
- Run "Limit Total" in weight paint mode to ensure that there is no more than 4 bones per vertex.

### HDR Colors
This engine will not follow the HDR FAD that for some reason certain graphics programmers accept without using logical reasoning. 
Colors are represented using a maximum of 8 bit per channel by default in the rendering code 
because display and camera light sensor technology currently and probably for the forseeable future can barely reach 8 bit depth in the real world already so having higher bit depth colours just adds bandwidth overhead for no reason.


### PBR Shading
Because I do not have or know anyone with a math degree I am dependent on tutorials for the lighting equations. This means the frame graph PBR lighting might
not be fully accurate and have missing features.