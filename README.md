<div>
  <img alt="CODE_LINES" src="https://img.shields.io/github/last-commit/vetux/xng" align="left">
  <img alt="LICENSE" src="https://img.shields.io/github/license/vetux/xng" align="left">
</div>

<br>

# xEngine - Game Engine library for C++

The engine is currently under heavy construction and should not be used in projects yet because the interface is still unstable and may change any time.

xEngine is designed to be a standalone modular toolkit for game development. Games can be created entirely in C++. The [editor](https://github.com/vetux/xng-editor) builds ontop of the library to provide a user friendly interface for authoring ECS scenes and game compilation (Cross Compilation, etc.). Users may also create their own specialized tools that link against the engine library.

Once the project has reached a stable state there will be semantically versioned releases (v0.1.0) and a stable branch (master) will be maintained.

[xng-editor](https://github.com/vetux/xng-editor) - Editor application to simplify and accelerate the game creation
workflow

## Features
 - High Performance Renderer using GPU Driven rendering techniques
 - Render Graph abstraction with explicit resource dependency tracking
 - Custom shader DSL for writing shaders in C++ ([ShaderScript](engine/include/xng/shaderscript))
 - Asynchronous Streaming for Renderer Objects
 - Pool based Entity Component System (ECS)
 - Cross Platform Adapter interfaces (Physics, Audio, Display, RenderGraph, etc.)
 - Resource System for loading and managing assets.

## Feature Checklist for Version 0.1.0
- Graphics
  - Render Graph
    - [X] OpenGL Implementation 
    - [ ] Vulkan Implementation 
  - 3D
    - [X] PBR Shading Model
    - [X] Skeletal Animation
    - [X] Shadow Mapping
    - [X] Image Based Lighting
    - [ ] Environment Mapping (Probes) 
    - [ ] Skybox Rendering 
    - [ ] Particles Rendering
    - [ ] Terrain Rendering
    - [ ] Volumetric Rendering
    - [ ] Post Processing (Anti Aliasing)
  - 2D
    - [X] Canvas Rendering
    - [X] Text Rendering
    - [ ] Sprite Animation
    - [ ] Sprite Rendering 
    - [ ] Sprite Lighting
- Resource Management
  - [X] Resource System
  - [ ] Custom Asset File format
- GUI
  - [ ] Flexbox based GUI Layout
 
## Screenshots (Old, will be updated when the Renderer rewrite is complete)
![](https://raw.githubusercontent.com/vetux/xng-assets/refs/heads/master/screenshots/ScreenshotFramegraph.jpg)

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
### Blender FBX Export
Set "Apply Scalings" to "FBX All""
Set "Forward" to "-Z Forward" and "Up" to "Y Up
Check "Use Space Transform" to generate the coordinate space conversion transformations in the exported nodes.
Check "Tangent Space" in the exporter "Geometry" options to generate tangents and bitangents for the exported mesh.
For rigged meshes run "Limit Total" in weight paint mode to ensure that there is no more than 4 bones per vertex

When exporting smoothed meshes make sure that any normal maps were generated using the smoothed mesh because Normals depend on the mesh smoothing on bake.
The engine also can calculate smoothed normals for meshes that are exported without smooth normals.

### 3D Math
Because I do not have or know anyone with a math degree I am dependent on tutorials and llms for the 3d math equations.
I would welcome any math geniuses to help me out once the engine has reached a more stable state.
