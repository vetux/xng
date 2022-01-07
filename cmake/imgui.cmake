include_directories(submodules/imgui/)

file(GLOB IMGUI.SRC submodules/imgui/*.cpp)
set(IMGUI.SRC ${IMGUI.SRC} submodules/imgui/backends/imgui_impl_opengl3.cpp
        submodules/imgui/backends/imgui_impl_glfw.cpp)

add_library(imgui STATIC ${IMGUI.SRC})