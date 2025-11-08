/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "instructioncompiler.hpp"

#include "xng/util/downcast.hpp"

#include "literals.hpp"
#include "types.hpp"

namespace InstructionCompiler {
    std::string compile(const ShaderInstruction &instruction,
                        const Shader &source,
                        const std::string &functionName,
                        const std::string &indent) {
        switch (instruction.code) {
            default:
                throw std::runtime_error("Unknown instruction code");
            case ShaderInstruction::DeclareVariable:
                return compileDeclareVariable(instruction, source, functionName, indent);
            case ShaderInstruction::Assign:
                return compileAssign(instruction, source, functionName, indent);
            case ShaderInstruction::Branch:
                return compileBranch(instruction, source, functionName, indent);
            case ShaderInstruction::Loop:
                return compileLoop(instruction, source, functionName, indent);
            case ShaderInstruction::CallFunction:
                return compileCall(instruction, source, functionName, indent);
            case ShaderInstruction::Return:
                return compileReturn(instruction, source, functionName, indent);
            case ShaderInstruction::EmitVertex:
                return compileEmitVertex(instruction, source, functionName, indent);
            case ShaderInstruction::EndPrimitive:
                return compileEndPrimitive(instruction, source, functionName, indent);
            case ShaderInstruction::SetFragmentDepth:
                return compileSetFragmentDepth(instruction, source, functionName, indent);
            case ShaderInstruction::SetLayer:
                return compileSetLayer(instruction, source, functionName, indent);
            case ShaderInstruction::SetVertexPosition:
                return compileSetVertexPosition(instruction, source, functionName, indent);
            case ShaderInstruction::VectorSwizzle:
                return compileVectorSwizzle(instruction, source, functionName, indent);
            case ShaderInstruction::ArraySubscript:
                return compileArraySubscript(instruction, source, functionName, indent);
            case ShaderInstruction::MatrixSubscript:
                return compileMatrixSubscript(instruction, source, functionName, indent);
            case ShaderInstruction::ObjectMember:
                return compileObjectElement(instruction, source, functionName, indent);
            case ShaderInstruction::CreateArray:
                return compileCreateArray(instruction, source, functionName, indent);
            case ShaderInstruction::CreateMatrix:
                return compileCreateMatrix(instruction, source, functionName, indent);
            case ShaderInstruction::CreateVector:
                return compileCreateVector(instruction, source, functionName, indent);
            case ShaderInstruction::CreateStruct:
                return compileCreateStruct(instruction, source, functionName, indent);
            case ShaderInstruction::TextureFetch:
                return compileTextureFetch(instruction, source, functionName, indent);
            case ShaderInstruction::TextureFetchArray:
                return compileTextureFetchArray(instruction, source, functionName, indent);
            case ShaderInstruction::TextureFetchMS:
                return compileTextureFetchMS(instruction, source, functionName, indent);
            case ShaderInstruction::TextureFetchMSArray:
                return compileTextureFetchMSArray(instruction, source, functionName, indent);
            case ShaderInstruction::TextureSample:
                return compileTextureSample(instruction, source, functionName, indent);
            case ShaderInstruction::TextureSampleArray:
                return compileTextureSampleArray(instruction, source, functionName, indent);
            case ShaderInstruction::TextureSampleCubeMap:
                return compileTextureSampleCubeMap(instruction, source, functionName, indent);
            case ShaderInstruction::TextureSampleCubeMapArray:
                return compileTextureSampleCubeMapArray(instruction, source, functionName, indent);
            case ShaderInstruction::TextureSize:
                return compileTextureSize(instruction, source, functionName, indent);
            case ShaderInstruction::BufferSize:
                return compileBufferSize(instruction, source, functionName, indent);
            case ShaderInstruction::Add:
            case ShaderInstruction::Subtract:
            case ShaderInstruction::Multiply:
            case ShaderInstruction::Divide:
            case ShaderInstruction::LogicalAnd:
            case ShaderInstruction::LogicalOr:
            case ShaderInstruction::GreaterEqual:
            case ShaderInstruction::Greater:
            case ShaderInstruction::LessEqual:
            case ShaderInstruction::Less:
            case ShaderInstruction::Equal:
            case ShaderInstruction::NotEqual:
                return compileArithmetic(instruction, source, functionName, indent);
            case ShaderInstruction::Abs:
            case ShaderInstruction::Sin:
            case ShaderInstruction::Cos:
            case ShaderInstruction::Tan:
            case ShaderInstruction::Asin:
            case ShaderInstruction::Acos:
            case ShaderInstruction::Atan:
            case ShaderInstruction::Pow:
            case ShaderInstruction::Exp:
            case ShaderInstruction::Log:
            case ShaderInstruction::Sqrt:
            case ShaderInstruction::InverseSqrt:
            case ShaderInstruction::Floor:
            case ShaderInstruction::Ceil:
            case ShaderInstruction::Round:
            case ShaderInstruction::Fract:
            case ShaderInstruction::Mod:
            case ShaderInstruction::Min:
            case ShaderInstruction::Max:
            case ShaderInstruction::Clamp:
            case ShaderInstruction::Mix:
            case ShaderInstruction::Step:
            case ShaderInstruction::SmoothStep:
            case ShaderInstruction::Dot:
            case ShaderInstruction::Cross:
            case ShaderInstruction::Normalize:
            case ShaderInstruction::Length:
            case ShaderInstruction::Distance:
            case ShaderInstruction::Reflect:
            case ShaderInstruction::Refract:
            case ShaderInstruction::FaceForward:
            case ShaderInstruction::Transpose:
            case ShaderInstruction::Inverse:
                return compileCallBuiltIn(instruction, source, functionName, indent);
        }
    }

    std::string compileOperand(const ShaderOperand &operand, const Shader &source, const std::string &functionName) {
        switch (operand.type) {
            default:
                throw std::runtime_error("Unknown operand type");
            case ShaderOperand::None:
                throw std::runtime_error("Unassigned operand");
            case ShaderOperand::Instruction:
                return compile(operand.instruction, source, functionName, {});
            case ShaderOperand::Buffer:
                if (source.buffers.at(operand.name).dynamic)
                    return bufferPrefix + operand.name + "." + bufferArrayName;
                return bufferPrefix + operand.name + "." + bufferArrayName + "[" + drawID + "]";
            case ShaderOperand::Texture:
                return texturePrefix + operand.name;
            case ShaderOperand::Parameter:
                return parameterPrefix + operand.name;
            case ShaderOperand::InputAttribute:
                return inputAttributePrefix + operand.name;
            case ShaderOperand::OutputAttribute:
                return outputAttributePrefix + operand.name;
            case ShaderOperand::Argument:
                return operand.name;
            case ShaderOperand::Variable:
                return operand.name;
            case ShaderOperand::Literal:
                return literalToString(operand.literal);
        }
    }

    std::string compileDeclareVariable(const ShaderInstruction &instruction,
                                       const Shader &source,
                                       const std::string &functionName,
                                       const std::string &indent) {
        std::string ret;
        if (std::holds_alternative<ShaderDataType>(instruction.type)) {
            auto t = std::get<ShaderDataType>(instruction.type);
            ret += getTypeName(t) + " " + instruction.name;
            if (t.count > 1) {
                ret += "[" + std::to_string(t.count) + "]";
            }
        } else {
            ret += std::get<ShaderStructTypeName>(instruction.type) + " " + instruction.name;
        }

        if (instruction.operands.at(0).isAssigned()) {
            ret += " = " + compileOperand(instruction.operands.at(0), source, functionName);
        }
        return indent + ret;
    }

    std::string compileAssign(const ShaderInstruction &instruction,
                              const Shader &source,
                              const std::string &functionName,
                              const std::string &indent) {
        return indent + compileOperand(instruction.operands.at(0), source, functionName) + " = "
               + compileOperand(instruction.operands.at(1), source, functionName);
    }

    std::string compileCreateVector(const ShaderInstruction &instruction,
                                    const Shader &source,
                                    const std::string &functionName,
                                    const std::string &indent) {
        std::string ret = getTypeName(std::get<ShaderDataType>(instruction.type)) + "(";
        for (auto &operand: instruction.operands) {
            if (!operand.isAssigned()) {
                break;
            }
            ret += compileOperand(operand, source, functionName) + ", ";
        }
        ret.pop_back();
        ret.pop_back();
        return ret + ")";
    }

    std::string compileCreateStruct(const ShaderInstruction &instruction,
                                    const Shader &source,
                                    const std::string &functionName,
                                    const std::string &indent) {
        return std::get<ShaderStructTypeName>(instruction.type) + "()";
    }

    std::string compileCreateMatrix(const ShaderInstruction &instruction,
                                    const Shader &source,
                                    const std::string &functionName,
                                    const std::string &indent) {
        std::string ret = getTypeName(std::get<ShaderDataType>(instruction.type)) + "(";
        for (auto &operand: instruction.operands) {
            if (!operand.isAssigned()) {
                break;
            }
            ret += compileOperand(operand, source, functionName) + ", ";
        }
        ret.pop_back();
        ret.pop_back();
        return ret + ")";
    }

    std::string compileCreateArray(const ShaderInstruction &instruction,
                                   const Shader &source,
                                   const std::string &functionName,
                                   const std::string &indent) {
        std::string ret;
        ret += getTypeName(std::get<ShaderDataType>(instruction.type));
        ret += "[](";
        for (auto &operand: instruction.operands) {
            ret += compileOperand(operand, source, functionName) + ", ";
        }
        ret.pop_back();
        ret.pop_back();
        ret += ")";
        return ret;
    }

    std::string compileTextureSample(const ShaderInstruction &instruction,
                                     const Shader &source,
                                     const std::string &functionName,
                                     const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        coords = "vec2(" + coords + ".x, 1 - " + coords + ".y)";
        if (instruction.operands.at(2).isAssigned()) {
            auto lod = compileOperand(instruction.operands.at(2), source, functionName);
            return "texture(" + name + ", " + coords + ", " + lod + ")";
        }
        return "texture(" + name + ", " + coords + ")";
    }

    std::string compileTextureSampleArray(const ShaderInstruction &instruction,
                                          const Shader &source,
                                          const std::string &functionName,
                                          const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        coords = "vec3(" + coords + ".x, 1 - " + coords + ".y, " + coords + ".z)";
        if (instruction.operands.at(2).isAssigned()) {
            auto lod = compileOperand(instruction.operands.at(2), source, functionName);
            return "texture(" + name + ", " + coords + ", " + lod + ")";
        }
        return "texture(" + name + ", " + coords + ")";
    }

    std::string compileTextureSampleCubeMap(const ShaderInstruction &instruction,
                                            const Shader &source,
                                            const std::string &functionName,
                                            const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        if (instruction.operands.at(2).isAssigned()) {
            auto lod = compileOperand(instruction.operands.at(2), source, functionName);
            return "texture(" + name + ", " + coords + ", " + lod + ")";
        }
        return "texture(" + name + ", " + coords + ")";
    }

    std::string compileTextureSampleCubeMapArray(const ShaderInstruction &instruction,
                                                 const Shader &source,
                                                 const std::string &functionName,
                                                 const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        if (instruction.operands.at(2).isAssigned()) {
            auto lod = compileOperand(instruction.operands.at(2), source, functionName);
            return "texture(" + name + ", " + coords + ", " + lod + ")";
        }
        return "texture(" + name + ", " + coords + ")";
    }

    std::string compileTextureFetch(const ShaderInstruction &instruction,
                                    const Shader &source,
                                    const std::string &functionName,
                                    const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        auto lod = compileOperand(instruction.operands.at(2), source, functionName);
        auto sizeY = "textureSize(" + name + ", " + lod + ").y";
        coords = "ivec2(" + coords + ".x, " + sizeY + " - " + coords + ".y)";
        return "texelFetch("
               + name + ", "
               + coords + ", "
               + lod + ")";
    }

    std::string compileTextureFetchArray(const ShaderInstruction &instruction,
                                         const Shader &source,
                                         const std::string &functionName,
                                         const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        auto lod = compileOperand(instruction.operands.at(2), source, functionName);
        auto sizeY = "textureSize(" + name + ", " + lod + ").y";
        coords = "ivec3(" + coords + ".x, " + sizeY + " - " + coords + ".y, " + coords + ".z)";
        return "texelFetch("
               + name + ", "
               + coords + ", "
               + lod + ")";
    }

    std::string compileTextureFetchMS(const ShaderInstruction &instruction,
                                      const Shader &source,
                                      const std::string &functionName,
                                      const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        auto sample = compileOperand(instruction.operands.at(2), source, functionName);
        auto sizeY = "textureSize(" + name + ").y";
        coords = "ivec2(" + coords + ".x, " + sizeY + " - " + coords + ".y)";
        return "texelFetch("
               + name + ", "
               + coords + ", "
               + sample + ")";
    }

    std::string compileTextureFetchMSArray(const ShaderInstruction &instruction,
                                           const Shader &source,
                                           const std::string &functionName,
                                           const std::string &indent) {
        auto name = compileOperand(instruction.operands.at(0), source, functionName);
        auto coords = compileOperand(instruction.operands.at(1), source, functionName);
        auto sample = compileOperand(instruction.operands.at(2), source, functionName);
        auto sizeY = "textureSize(" + name + ").y";
        coords = "ivec3(" + coords + ".x, " + sizeY + " - " + coords + ".y, " + coords + ".z)";
        return "texelFetch("
               + name + ", "
               + coords + ", "
               + sample + ")";
    }

    std::string compileTextureSize(const ShaderInstruction &instruction,
                                   const Shader &source,
                                   const std::string &functionName,
                                   const std::string &indent) {
        if (instruction.operands.at(1).isAssigned()) {
            return "textureSize("
                   + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                   + compileOperand(instruction.operands.at(1), source, functionName) + ")";
        }
        return "textureSize(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
    }

    std::string compileBufferSize(const ShaderInstruction &instruction,
                                  const Shader &source,
                                  const std::string &functionName,
                                  const std::string &indent) {
        return bufferPrefix + instruction.name + "." + bufferArrayName + ".length()";
    }

    std::string compileArithmetic(const ShaderInstruction &instruction,
                                  const Shader &source,
                                  const std::string &functionName,
                                  const std::string &indent) {
        std::string ret;
        switch (instruction.code) {
            default:
                throw std::runtime_error("Unknown arithmetic operator");
            case ShaderInstruction::Add:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " + "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::Subtract:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " - "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::Multiply:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " * "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::Divide:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " / "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::LogicalAnd:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " && "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::LogicalOr:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " || "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::GreaterEqual:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " >= "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::Greater:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " > "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::LessEqual:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " <= "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::Less:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " < "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::Equal:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " == "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
            case ShaderInstruction::NotEqual:
                ret = compileOperand(instruction.operands.at(0), source, functionName)
                      + " != "
                      + compileOperand(instruction.operands.at(1), source, functionName);
                break;
        }
        return "(" + ret + ")";
    }

    std::string compileCall(const ShaderInstruction &instruction,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &indent) {
        std::string args;
        for (auto &operand: instruction.operands) {
            if (operand.isAssigned()) {
                args += compileOperand(operand, source, functionName) + ", ";
            } else {
                break;
            }
        }
        if (args.size() > 0) {
            args.pop_back();
            args.pop_back();
        }
        return instruction.name + "(" + args + ")";
    }

    std::string compileReturn(const ShaderInstruction &instruction,
                              const Shader &source,
                              const std::string &functionName,
                              const std::string &indent) {
        std::string ret = indent + "return ";
        if (instruction.operands.at(0).isAssigned()) {
            ret += compileOperand(instruction.operands.at(0), source, functionName);
        }
        return ret;
    }

    std::string compileCallBuiltIn(const ShaderInstruction &instruction,
                                   const Shader &source,
                                   const std::string &functionName,
                                   const std::string &indent) {
        switch (instruction.code) {
            default:
                throw std::runtime_error("Unknown built-in function");
            case ShaderInstruction::Abs:
                return "abs(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Sin:
                return "sin(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Cos:
                return "cos(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Tan:
                return "tan(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Asin:
                return "asin(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Acos:
                return "acos(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Atan:
                return "atan(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Pow:
                return "pow(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Exp:
                return "exp(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Log:
                return "log(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Sqrt:
                return "sqrt(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::InverseSqrt:
                return "inversesqrt(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Floor:
                return "floor(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Ceil:
                return "ceil(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Round:
                return "round(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Fract:
                return "fract(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Mod:
                return "mod(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Min:
                return "min(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Max:
                return "max(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Clamp:
                return "clamp(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(2), source, functionName) + ")";
            case ShaderInstruction::Mix:
                return "mix(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(2), source, functionName) + ")";
            case ShaderInstruction::Step:
                return "step(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::SmoothStep:
                return "smoothstep(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(2), source, functionName) + ")";
            case ShaderInstruction::Dot:
                return "dot(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Cross:
                return "cross(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Normalize:
                return "normalize(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Length:
                return "length(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
            case ShaderInstruction::Distance:
                return "distance(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Reflect:
                return "reflect(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ")";
            case ShaderInstruction::Refract:
                return "refract(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(2), source, functionName) + ")";
            case ShaderInstruction::FaceForward:
                return "faceforward(" + compileOperand(instruction.operands.at(0), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(1), source, functionName) + ", "
                       + compileOperand(instruction.operands.at(2), source, functionName) + ")";
            case ShaderInstruction::Transpose:
                return "transpose(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";

            case ShaderInstruction::Inverse:
                return "inverse(" + compileOperand(instruction.operands.at(0), source, functionName) + ")";
        }
    }

    std::string compileArraySubscript(const ShaderInstruction &instruction,
                                      const Shader &source,
                                      const std::string &functionName,
                                      const std::string &indent) {
        return compileOperand(instruction.operands.at(0), source, functionName) + "[" + compileOperand(
                   instruction.operands.at(1), source, functionName) + "]";
    }

    std::string compileVectorSwizzle(const ShaderInstruction &instruction,
                                     const Shader &source,
                                     const std::string &functionName,
                                     const std::string &indent) {
        if (instruction.components.size() < 1 || instruction.components.size() > 4) {
            throw std::runtime_error("Invalid vector subscript indices size");
        }
        std::string ret = compileOperand(instruction.operands.at(0), source, functionName) + ".";
        for (auto &index: instruction.components) {
            switch (index) {
                case ShaderInstruction::COMPONENT_x:
                    ret += "x";
                    break;
                case ShaderInstruction::COMPONENT_y:
                    ret += "y";
                    break;
                case ShaderInstruction::COMPONENT_z:
                    ret += "z";
                    break;
                case ShaderInstruction::COMPONENT_w:
                    ret += "w";
                    break;
                default:
                    throw std::runtime_error("Invalid vector subscript index");
            }
        }
        return ret;
    }

    std::string compileMatrixSubscript(const ShaderInstruction &instruction,
                                       const Shader &source,
                                       const std::string &functionName,
                                       const std::string &indent) {
        auto ret = compileOperand(instruction.operands.at(0), source, functionName) + "[" + compileOperand(
                       instruction.operands.at(1), source, functionName) + "]";
        if (instruction.operands.at(2).isAssigned()) {
            ret += "[" + compileOperand(instruction.operands.at(2), source, functionName) + "]";
        }
        return ret;
    }

    std::string compileBranch(const ShaderInstruction &instruction,
                              const Shader &source,
                              const std::string &functionName,
                              const std::string &indent) {
        std::string ret = indent + "if(" + compileOperand(instruction.operands.at(0), source, functionName) + ") {\n";
        for (auto &inst: instruction.branchA) {
            ret += compile(inst, source, functionName, indent + "\t") + ";\n";
        }
        ret += indent + "}";
        if (instruction.branchB.size() > 0) {
            ret += " else {\n";
            for (auto &inst: instruction.branchB) {
                ret += compile(inst, source, functionName, indent + "\t") + ";\n";
            }
            ret += indent + "}";
        }
        return ret;
    }

    std::string compileLoop(const ShaderInstruction &instruction,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &indent) {
        std::string ret = indent + "for(" + compileOperand(instruction.operands.at(0), source, functionName) + "; "
                          + compileOperand(instruction.operands.at(1), source, functionName) + "; "
                          + compileOperand(instruction.operands.at(2), source, functionName) + ") {\n";
        for (auto &inst: instruction.branchA) {
            ret += compile(inst, source, functionName, indent + "\t") + ";\n";
        }
        ret += indent + "}";
        return ret;
    }

    std::string compileSetVertexPosition(const ShaderInstruction &instruction,
                                         const Shader &source,
                                         const std::string &functionName,
                                         const std::string &indent) {
        return indent + "gl_Position = " + compileOperand(instruction.operands.at(0), source, functionName);
    }

    std::string compileSetFragmentDepth(const ShaderInstruction &instruction,
                                        const Shader &source,
                                        const std::string &functionName,
                                        const std::string &indent) {
        return indent + "gl_FragDepth = " + compileOperand(instruction.operands.at(0), source, functionName);
    }

    std::string compileSetLayer(const ShaderInstruction &instruction,
                                const Shader &source,
                                const std::string &functionName,
                                const std::string &indent) {
        return indent + "gl_Layer = " + compileOperand(instruction.operands.at(0), source, functionName);
    }

    std::string compileEmitVertex(const ShaderInstruction &instruction,
                                  const Shader &source,
                                  const std::string &functionName,
                                  const std::string &indent) {
        return indent + "EmitVertex()";
    }

    std::string compileEndPrimitive(const ShaderInstruction &instruction,
                                    const Shader &source,
                                    const std::string &functionName,
                                    const std::string &indent) {
        return indent + "EndPrimitive()";
    }

    std::string compileObjectElement(const ShaderInstruction &instruction,
                                     const Shader &source,
                                     const std::string &functionName,
                                     const std::string &indent) {
        return compileOperand(instruction.operands.at(0), source, functionName) + "." + instruction.name;
    }
}
