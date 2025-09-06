#ifndef XENGINE_FGSHADERDECOMPILER_HPP
#define XENGINE_FGSHADERDECOMPILER_HPP

#include <vector>

#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    /**
     * The shader decompiler creates the internal representation of the given shader in some language (eg. GLSL)
     *
     * Because of limitations of the internal representation where it might not match all features of languages such as GLSL
     * this decompiler would be meant for a possible future custom shading language for this engine. (Like ShaderLab in Unity)
     */
    class FGShaderDecompiler {
    public:
        virtual ~FGShaderDecompiler() = default;

        virtual FGShaderSource decompile(const std::string &shader) = 0;
    };
}

#endif //XENGINE_FGSHADERDECOMPILER_HPP
