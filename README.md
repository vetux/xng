# xEngine
xEngine is a cross-platform c++ game engine library.

## Features
- Interface based Driver Abstractions
- Frame Graph Renderer (WIP)
- 2D Renderer
- Skeletal Animation (WIP)
- Sprite Animation
- PAK file format
- ECS with data oriented architecture

## Programs
- [xEditor](https://github.com/vetux/xeditor)
- [xSamples](https://github.com/vetux/xsamples)
- [GameOfLife](https://github.com/vetux/gameoflife)

# Building
- Ensure the external dependencies are provided. ([install_dependencies.sh](install_dependencies.sh]))
- Clone the [imgui](submodules/imgui) submodule
- Load [CMakeLists.txt](CMakeLists.txt) and build the engine target