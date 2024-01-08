set(SHADER_HEADERS)

file(GLOB LIB_SHADERS "${BASE_SOURCE_DIR}/shaders/lib/*.glsl")

function(CompileShader PATH STAGE ENTRYPOINT)
    add_custom_command(
            OUTPUT ${SHADER_COMPILED_DIR}${PATH}.hpp
            DEPENDS shadercompiler ${BASE_SOURCE_DIR}/shaders/${PATH}.glsl ${LIB_SHADERS}
            COMMAND shadercompiler -m header -f -i ${BASE_SOURCE_DIR}/shaders/lib/ -p -l GLSL_460 -s ${STAGE} -e main ${BASE_SOURCE_DIR}/shaders/${PATH}.glsl ${SHADER_COMPILED_DIR}${PATH}.hpp)
    set(SHADER_HEADERS ${SHADER_HEADERS} ${SHADER_COMPILED_DIR}${PATH}.hpp PARENT_SCOPE)
endfunction()

CompileShader(graph/constructionpass_vs VERTEX main)
CompileShader(graph/constructionpass_vs_skinned VERTEX main)
CompileShader(graph/constructionpass_fs FRAGMENT main)

CompileShader(graph/deferredlightingpass_vs VERTEX main)
CompileShader(graph/deferredlightingpass_fs FRAGMENT main)

CompileShader(graph/forwardlightingpass_vs VERTEX main)
CompileShader(graph/forwardlightingpass_fs FRAGMENT main)

CompileShader(graph/compositepass_vs VERTEX main)
CompileShader(graph/compositepass_fs FRAGMENT main)

CompileShader(graph/shadowmappingpass_vs VERTEX main)
CompileShader(graph/shadowmappingpass_fs FRAGMENT main)
CompileShader(graph/shadowmappingpass_gs GEOMETRY main)
CompileShader(graph/shadowmappingpass_dir_vs VERTEX main)
CompileShader(graph/shadowmappingpass_dir_fs FRAGMENT main)
CompileShader(graph/shadowmappingpass_dir_gs GEOMETRY main)

CompileShader(graph/skyboxpass_vs VERTEX main)
CompileShader(graph/skyboxpass_fs FRAGMENT main)

CompileShader(graph/wireframepass_vs VERTEX main)
CompileShader(graph/wireframepass_fs FRAGMENT main)

CompileShader(ren2d/vs_multi VERTEX main)
CompileShader(ren2d/fs_multi FRAGMENT main)

CompileShader(ren2d/vs VERTEX main)
CompileShader(ren2d/fs FRAGMENT main)

CompileShader(tests/debugpass_vs VERTEX main)
CompileShader(tests/debugpass_fs FRAGMENT main)

CompileShader(tests/mandelbrot COMPUTE main)

list(LENGTH SHADER_HEADERS LEN_SHADER_HEADERS)

message("${LEN_SHADER_HEADERS} Precompiled Shaders")
foreach (val IN LISTS SHADER_HEADERS)
    message("   ${val}")
endforeach ()