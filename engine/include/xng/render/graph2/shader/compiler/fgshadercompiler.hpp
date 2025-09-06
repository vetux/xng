#ifndef XENGINE_FGSHADERCOMPILER_HPP
#define XENGINE_FGSHADERCOMPILER_HPP

#include <vector>

#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    /**
     * The shader compiler creates source code in some language (eg. GLSL) from the given internal representation.
     */
    class FGShaderCompiler {
    public:
        virtual ~FGShaderCompiler() = default;

        virtual std::string compile(const FGShaderSource &source) = 0;
    };
}

#endif //XENGINE_FGSHADERCOMPILER_HPP