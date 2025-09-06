#include "xng/render/graph2/shader/compiler/fgshadercompilerglsl.hpp"

namespace xng {
    std::string FGShaderCompilerGLSL::compile(const FGShaderSource &source) {
        std::string ret;
        for (const auto& func : source.functions) {
            ret += generateFunction(func.first, func.second);
        }
        ret += generateFunction("main", source.operations);
        return ret;
    }

    std::string FGShaderCompilerGLSL::generateFunction(const std::string &functionName,
        const std::vector<FGShaderOperation> &operations) {
        std::string ret = "void " + functionName + "() {";
         for (size_t i = 0; i < operations.size(); i = i + 1) {
            auto op = operations[i];
            switch (op.operation) {
                case FGShaderOperation::ATTRIBUTE_READ:
                    break;
                case FGShaderOperation::ATTRIBUTE_WRITE:
                    break;
                case FGShaderOperation::PARAMETER_READ:
                    break;
                case FGShaderOperation::PARAMETER_WRITE:
                    break;
                case FGShaderOperation::TEXTURE_READ:
                    break;
                case FGShaderOperation::TEXTURE_WRITE:
                    break;
                case FGShaderOperation::ASSIGN:
                    break;
                case FGShaderOperation::ADD:
                    break;
                case FGShaderOperation::SUBTRACT:
                    break;
                case FGShaderOperation::MULTIPLY:
                    break;
                case FGShaderOperation::DIVIDE:
                    break;
                case FGShaderOperation::COMPARE_EQUAL:
                    break;
                case FGShaderOperation::COMPARE_NEQUAL:
                    break;
                case FGShaderOperation::COMPARE_MORE:
                    break;
                case FGShaderOperation::COMPARE_LESS:
                    break;
                case FGShaderOperation::COMPARE_MORE_EQUAL:
                    break;
                case FGShaderOperation::COMPARE_LESS_EQUAL:
                    break;
                case FGShaderOperation::LOGICAL_AND:
                    break;
                case FGShaderOperation::LOGICAL_OR:
                    break;
                case FGShaderOperation::CALL:
                    break;
                case FGShaderOperation::RETURN:
                    break;
                case FGShaderOperation::SUBSCRIPT:
                    break;
                case FGShaderOperation::CONDITIONAL:
                    break;
                case FGShaderOperation::LOOP:
                    break;
                default:
                    throw std::runtime_error("unsupported shader operation");
            }
        }
        return ret + "}";
    }
}
