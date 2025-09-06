#ifndef XENGINE_FGSHADERCOMPILERGLSL_HPP
#define XENGINE_FGSHADERCOMPILERGLSL_HPP

#include "xng/render/graph2/shader/compiler/fgshadercompiler.hpp"

namespace xng {
    class FGShaderCompilerGLSL : public FGShaderCompiler {
    public:
        FGShaderCompilerGLSL() = default;

        std::string compile(const FGShaderSource &source) override;

    private:
        std::string generateFunction(const std::string &functionName, const std::vector<FGShaderOperation> &operations);
    };
}

#endif //XENGINE_FGSHADERCOMPILERGLSL_HPP
