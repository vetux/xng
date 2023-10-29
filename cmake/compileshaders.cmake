set(SHADER_HEADERS)

function(CompileShader PATH STAGE ENTRYPOINT)
    add_custom_command(
            OUTPUT ${BASE_SOURCE_DIR}/shaders/generated/${PATH}.hpp
            DEPENDS shadercompiler ${BASE_SOURCE_DIR}/shaders/${PATH}.glsl
            COMMAND shadercompiler -m header -f -i ${BASE_SOURCE_DIR}/shaders/lib/ -p -l GLSL_460 -s ${STAGE} -e main ${BASE_SOURCE_DIR}/shaders/${PATH}.glsl ${BASE_SOURCE_DIR}/shaders/generated/${PATH}.hpp)
    set(SHADER_HEADERS ${SHADER_HEADERS} ${BASE_SOURCE_DIR}/shaders/generated/${PATH}.hpp PARENT_SCOPE)
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

CompileShader(ren2d/vs_multi VERTEX main)
CompileShader(ren2d/fs_multi FRAGMENT main)

CompileShader(ren2d/vs VERTEX main)
CompileShader(ren2d/fs FRAGMENT main)

CompileShader(testpass/vs VERTEX main)
CompileShader(testpass/fs FRAGMENT main)

CompileShader(mandelbrot/mandelbrot COMPUTE main)

list(LENGTH SHADER_HEADERS LEN_SHADER_HEADERS)

message("${LEN_SHADER_HEADERS} Precompiled Shaders")
foreach (val IN LISTS SHADER_HEADERS)
    message("   ${val}")
endforeach ()