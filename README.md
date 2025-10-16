<div>
  <img alt="CODE_LINES" src="https://img.shields.io/github/last-commit/vetux/xng" align="left">
  <img alt="LICENSE" src="https://img.shields.io/github/license/vetux/xng" align="left">
</div>

<br>

# xEngine

xEngine is a Game Engine library for C++. 

The engine is designed as a standalone modular toolkit from which different modules can be used independently.

This project is currently under heavy construction and should not be used in projects yet because the interface is still unstable and may change any time.

Once the project has reached a stable state there will be semantically versioned releases.

[xng-editor](https://github.com/vetux/xng-editor) - Editor application to simplify and accelerate the game creation
workflow

## Features
- Graphics
  - Render Graph
    - Domain-Specific Language for writing Render Graph shaders in C++
  - 3D
    - PBR Render Graph Pipeline
      - Forward / Deferred Rendering
      - Shadow Mapping
    - Skeletal Animation
  - 2D
    - Canvas Interface
    - Sprite Animation
    - Text Rendering
- Resource Management
  - Uri Resource addressing
  - Multithreaded Resource streaming
  - Automatic Resource Lifetime handling
- Entity Component System interface implementing data-oriented architecture
- PAK file format with compression, chunking and encryption support

## Todolist for Version 0.1.0
- Graphics
  - Render Graph
    - [ ] Vulkan Implementation 
  - 3D 
    - [ ] Refine the PBR Shading Model 
    - [ ] Refine Shadow Mapping Implementation
    - [ ] Skybox Rendering 
    - [ ] Particles Rendering
    - [ ] Terrain Rendering
    - [ ] Volumetric Rendering
    - [ ] Image Based Lighting
    - [ ] Environment Mapping (Reflections)
    - [ ] Post Processing (Anti Aliasing)
  - 2D
    - [ ] Sprite Rendering 
    - [ ] Sprite Lighting
- [ ] Custom Asset File format
- [ ] Flexbox based GUI Layout
 
## Screenshots (Old, will be updated when the pbr shading model is fixed up)
![](https://raw.githubusercontent.com/vetux/xng-assets/refs/heads/master/screenshots/ScreenshotFramegraph.jpg)

## Adapters

Adapters are implementations of interfaces in the xEngine API (For example through 3rd party libraries) that can be optionally compiled into the engine library. If an adapter type is instantiated in an application, the corresponding adapter must be defined by the linked engine library by enabling the corresponding cmake option/s in [adapters.cmake](cmake/adapters.cmake) when compiling the engine library.

- Display Environment
    - GLFW
    - Android (WIP)
- 3D Audio
    - OpenAL-Soft
- Render Graph
    - OpenGL
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
When exporting rigged meshes from blender to be imported with the assimp importer platform make sure to:
- Set the "Apply Scalings" to fbx unit scale.
- Rotate the mesh and the rig -90 degrees in the X axis and apply the rotation.
- Set the forward and up axis in the fbx exporter settings to Y forward and Z up to match blenders coordinate system.
- Uncheck "Key All Bones" and "Force Start/End Keyframes"
  - "Key All Bones" appears to do nothing at all
  - "Force Start/End Keyframes" creates invalid keyframes inside an animation for bones which are not keyframed in the animation.
- Run "Limit Total" in weight paint mode to ensure that there is no more than 4 bones per vertex.

### PBR Shading
Because I do not have or know anyone with a math degree I am dependent on tutorials for the lighting calculations.

I would welcome any math geniuses to help me out once the engine has reached a more stable state.