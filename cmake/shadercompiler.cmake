# The shader compiler used for generating the shader header files for the engine.

add_executable(shadercompiler
        ${BASE_SOURCE_DIR}/shadercompiler/src/main.cpp
        ${BASE_SOURCE_DIR}/engine/src/shader/shaderdirectoryinclude.cpp
        ${BASE_SOURCE_DIR}/adapters/glslang/glslang.cpp)
target_include_directories(shadercompiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/shadercompiler/src/
        ${BASE_SOURCE_DIR}/engine/include/)
target_link_libraries(shadercompiler Threads::Threads glslang SPIRV glslang-default-resource-limits MachineIndependent OSDependent GenericCodeGen OGLCompiler)
