<div>
  <img alt="CODE_LINES" src="https://img.shields.io/github/last-commit/vetux/xng" align="left">
  <img alt="LICENSE" src="https://img.shields.io/github/license/vetux/xng" align="left">
</div>

<br>

# xEngine - Game Engine library for C++

This project aims to provide a modular high-performance toolkit for game development.

The engine is a standalone C++ library that can be used independently of the editor or other tools.

The [editor](https://github.com/vetux/xng-editor) builds ontop of the library to provide a graphical user interface for
creating games.
Users may also create their own standalone specialized tools that link against the engine library.

## ⚠️ Current Status

The engine is currently under heavy construction and should not be used in projects yet because the interface is still
unstable and may change any time.

Once the project has reached a stable state there will be semantically versioned releases (v0.1.0) and a stable branch (
master) will be maintained.

## Features

- High Performance Renderer using GPU Driven rendering techniques
- Virtual Texturing Implementation
- Render Graph abstraction with explicit resource dependency tracking
- Custom shader DSL for writing shaders in C++ ([ShaderScript](engine/include/xng/shaderscript))
- Pool based Entity Component System (ECS)
- Cross Platform Adapter interfaces (Physics, Audio, Display, RenderGraph, etc.)
- Resource System for loading and managing assets.

## Screenshots (Old, Will be updated soon with the new renderer)

![](https://raw.githubusercontent.com/vetux/xng-assets/refs/heads/master/screenshots/ScreenshotFramegraph.jpg)

## Building

### Supported Platforms

- Linux
    - Tested Compiler: GCC 12.2.0 64bit
- Windows
    - Tested Compilers:
        - MSVC 2022 64bit
            - Build warnings are emitted due to the fact that xng uses STL types in dllexport types.
            - Users must ensure that their application is built with the same compiler version as the linked library
              because
              windows uses multiple STL dll files and allows loading of multiple STL version dlls into the same process.
              If
              the user links to a different STL version than the one used for building the engine dll there can be
              problems
              when passing types through the dll interface.
        - MinGW-w64 11
- Android
    - Tested Compiler: Android NDK r25c

### Build Instructions

Check [BUILDING.md](BUILDING.md) for detailed building instructions.

## Notes

### Blender FBX Export > Assimp Importer

- Set "Apply Scalings" to "FBX All""
- Set "Forward" to "-Z Forward" and "Up" to "Y Up"
- Check "Use Space Transform" to generate the coordinate space conversion transformations in the exported nodes.
- Check "Tangent Space" in the exporter "Geometry" options to generate tangents and bitangents for the exported mesh.

For rigged meshes it is recommended to run "Limit Total" in weight paint mode to ensure that there is
no more than 4 bones per vertex. The mesh streamer only supports 4 bones per vertex, and in case there are more than
4 bones per vertex, the mesh streamer will select the 4 most influential bones based on the vertex weights.

When exporting smoothed meshes make sure that any normal maps were generated using the smoothed mesh because normal maps
depend on the mesh normals that were used on bake.

The engine also can calculate smoothed vertex normals for meshes that are exported with flat vertex normals.

### 3D Math

Because I do not have or know anyone with a math degree I am dependent on tutorials and llms for the 3d math equations.
I would welcome any math geniuses to help me out once the engine has reached a more stable state.
