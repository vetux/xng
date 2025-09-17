/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "nodecompiler.hpp"

#include "xng/util/downcast.hpp"

#include "literals.hpp"
#include "types.hpp"

std::string compileNode(const ShaderNode &node,
                        const Shader &source,
                        const std::string &functionName,
                        const std::string &prefix) {
    switch (node.getType()) {
        case ShaderNode::VARIABLE_CREATE:
            return compileLeafNode(down_cast<const NodeVariableCreate &>(node), source, functionName, prefix);
        case ShaderNode::ASSIGN:
            return compileLeafNode(down_cast<const NodeAssign &>(node), source, functionName, prefix);
        case ShaderNode::VARIABLE:
            return compileLeafNode(down_cast<const NodeVariable &>(node));
        case ShaderNode::LITERAL:
            return compileLeafNode(down_cast<const NodeLiteral &>(node));
        case ShaderNode::ARGUMENT:
            return compileLeafNode(down_cast<const NodeArgument &>(node));
        case ShaderNode::VECTOR:
            return compileLeafNode(down_cast<const NodeVector &>(node), source, functionName);
        case ShaderNode::ARRAY:
            return compileLeafNode(down_cast<const NodeArray &>(node), source, functionName);
        case ShaderNode::ATTRIBUTE_IN:
            return compileLeafNode(down_cast<const NodeAttributeInput &>(node));
        case ShaderNode::ATTRIBUTE_OUT:
            return compileLeafNode(down_cast<const NodeAttributeOutput &>(node));
        case ShaderNode::PARAMETER:
            return compileLeafNode(down_cast<const NodeParameter &>(node));
        case ShaderNode::TEXTURE:
            return compileLeafNode(down_cast<const NodeTexture &>(node), source, functionName);
        case ShaderNode::TEXTURE_SAMPLE:
            return compileLeafNode(down_cast<const NodeTextureSample &>(node), source, functionName);
        case ShaderNode::TEXTURE_FETCH:
            return compileLeafNode(down_cast<const NodeTextureFetch &>(node), source, functionName);
        case ShaderNode::TEXTURE_SIZE:
            return compileLeafNode(down_cast<const NodeTextureSize &>(node), source, functionName);
        case ShaderNode::BUFFER_READ:
            return compileLeafNode(down_cast<const NodeBufferRead &>(node), source, functionName);
        case ShaderNode::BUFFER_WRITE:
            return compileLeafNode(down_cast<const NodeBufferWrite &>(node), source, functionName, prefix);
        case ShaderNode::BUFFER_SIZE:
            return compileLeafNode(down_cast<const NodeBufferSize &>(node), source);
        case ShaderNode::ADD:
            return compileLeafNode(down_cast<const NodeAdd &>(node), source, functionName);
        case ShaderNode::SUBTRACT:
            return compileLeafNode(down_cast<const NodeSubtract &>(node), source, functionName);
        case ShaderNode::MULTIPLY:
            return compileLeafNode(down_cast<const NodeMultiply &>(node), source, functionName);
        case ShaderNode::DIVIDE:
            return compileLeafNode(down_cast<const NodeDivide &>(node), source, functionName);
        case ShaderNode::EQUAL:
            return compileLeafNode(down_cast<const NodeEqual &>(node), source, functionName);
        case ShaderNode::NEQUAL:
            return compileLeafNode(down_cast<const NodeNotEqual &>(node), source, functionName);
        case ShaderNode::GREATER:
            return compileLeafNode(down_cast<const NodeGreater &>(node), source, functionName);
        case ShaderNode::LESS:
            return compileLeafNode(down_cast<const NodeLess &>(node), source, functionName);
        case ShaderNode::GREATER_EQUAL:
            return compileLeafNode(down_cast<const NodeGreaterEqual &>(node), source, functionName);
        case ShaderNode::LESS_EQUAL:
            return compileLeafNode(down_cast<const NodeLessEqual &>(node), source, functionName);
        case ShaderNode::AND:
            return compileLeafNode(down_cast<const NodeAnd &>(node), source, functionName);
        case ShaderNode::OR:
            return compileLeafNode(down_cast<const NodeOr &>(node), source, functionName);
        case ShaderNode::CALL:
            return compileLeafNode(down_cast<const NodeCall &>(node), source, functionName);
        case ShaderNode::RETURN:
            return compileLeafNode(down_cast<const NodeReturn &>(node), source, functionName, prefix);
        case ShaderNode::BUILTIN:
            return compileLeafNode(down_cast<const NodeBuiltin &>(node), source, functionName);
        case ShaderNode::SUBSCRIPT_ARRAY:
            return compileLeafNode(down_cast<const NodeSubscriptArray &>(node), source, functionName);
        case ShaderNode::VECTOR_SWIZZLE:
            return compileLeafNode(down_cast<const NodeVectorSwizzle &>(node), source, functionName);
        case ShaderNode::SUBSCRIPT_MATRIX:
            return compileLeafNode(down_cast<const NodeSubscriptMatrix &>(node), source, functionName);
        case ShaderNode::BRANCH:
            return compileLeafNode(down_cast<const NodeBranch &>(node), source, functionName, prefix);
        case ShaderNode::LOOP:
            return compileLeafNode(down_cast<const NodeLoop &>(node), source, functionName, prefix);
        case ShaderNode::VERTEX_POSITION:
            return compileLeafNode(down_cast<const NodeVertexPosition &>(node), source, functionName, prefix);
    }
    throw std::runtime_error("Node Type not implemented");
}

std::string compileLeafNode(const NodeVariableCreate &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    auto ret = getTypeName(node.type) + " " + node.variableName;
    if (node.count > 1) {
        ret += "[" + std::to_string(node.count) + "]";
    }
    if (node.value != nullptr) {
        ret += " = " + compileNode(*node.value, source, functionName);
    }
    return prefix + ret;
}

std::string compileLeafNode(const NodeAssign &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    return prefix + compileNode(*node.target, source, functionName)
           + " = "
           + compileNode(*node.value, source, functionName);
}

std::string compileLeafNode(const NodeVariable &node) {
    return node.variableName;
}

std::string compileLeafNode(const NodeLiteral &node) {
    return literalToString(node.value);
}

std::string compileLeafNode(const NodeArgument &node) {
    return node.argumentName;
}

std::string compileLeafNode(const NodeVector &node,
                            const Shader &source,
                            const std::string &functionName) {
    std::string ret;
    ret += getTypeName(node.type);
    ret += "(";
    ret += compileNode(*node.x, source, functionName, "");
    ret += ", ";
    ret += compileNode(*node.y, source, functionName, "");
    if (node.z != nullptr) {
        ret += ", ";
        ret += compileNode(*node.z, source, functionName, "");
        if (node.w != nullptr) {
            ret += ", ";
            ret += compileNode(*node.w, source, functionName, "");
        }
    }
    ret += ")";
    return ret;
}

std::string compileLeafNode(const NodeMatrix &node, const Shader &source, const std::string &functionName) {
    std::string ret;
    ret += getTypeName(node.type);
    ret += "(";
    ret += compileNode(*node.x, source, functionName, "");
    ret += ", ";
    ret += compileNode(*node.y, source, functionName, "");
    if (node.z != nullptr) {
        ret += ", ";
        ret += compileNode(*node.z, source, functionName, "");
        if (node.w != nullptr) {
            ret += ", ";
            ret += compileNode(*node.w, source, functionName, "");
        }
    }
    ret += ")";
    return ret;
}

std::string compileLeafNode(const NodeArray &node, const Shader &source, const std::string &functionName) {
    std::string ret;
    ret += getTypeName(node.elementType);
    ret += "[](";
    size_t counter = 0;
    for (auto &value: node.values) {
        if (counter > 0) {
            ret += ", ";
        }
        ret += compileNode(*value, source, functionName);
        counter++;
    }
    ret += ")";
    return ret;
}

std::string compileLeafNode(const NodeAttributeInput &node) {
    return inputAttributePrefix + node.attributeName;
}

std::string compileLeafNode(const NodeAttributeOutput &node) {
    return outputAttributePrefix + node.attributeName;
}

std::string compileLeafNode(const NodeParameter &node) {
    return parameterPrefix + node.parameterName;
}

std::string compileLeafNode(const NodeTexture &node,
                            const Shader &source,
                            const std::string &functionName) {
    std::string texIndex = "0";
    if (node.textureIndex != nullptr) {
        texIndex = compileNode(*node.textureIndex, source, functionName);
    }
    return texturePrefix
           + node.textureName
           + "["
           + texIndex
           + "]";
}

std::string compileLeafNode(const NodeTextureSample &node,
                            const Shader &source,
                            const std::string &functionName) {
    auto ret = "texture("
               + compileNode(*node.texture, source, functionName)
               + ", "
               + compileNode(*node.coordinate, source, functionName, "");
    if (node.bias != nullptr) {
        ret += ", " + compileNode(*node.bias, source, functionName, "");
    }
    ret += ")";
    return ret;
}

std::string compileLeafNode(const NodeTextureFetch &node,
                            const Shader &source,
                            const std::string &functionName) {
    return "texelFetch("
           + compileNode(*node.texture, source, functionName)
           + ", "
           + compileNode(*node.coordinate, source, functionName, "")
           + ", "
           + compileNode(*node.index, source, functionName, "")
           + ")";
}

std::string compileLeafNode(const NodeTextureSize &node,
                            const Shader &source,
                            const std::string &functionName) {
    auto ret = "textureSize(" + compileNode(*node.texture, source, functionName);
    if (node.lod != nullptr) {
        ret += ", " + compileNode(*node.lod, source, functionName, "");
    }
    return ret + ")";
}

std::string compileLeafNode(const NodeBufferRead &node,
                            const Shader &source,
                            const std::string &functionName) {
    if (source.buffers.at(node.bufferName).dynamic) {
        auto ret = bufferPrefix + node.bufferName + "." + bufferArrayName + "[";
        ret += compileNode(*node.index, source, functionName, "");
        ret += "]." + node.elementName;
        return ret;
    } else {
        return bufferPrefix
               + node.bufferName
               + "."
               + bufferArrayName
               + "["
               + drawID
               + "]."
               + node.elementName;
    }
}

std::string compileLeafNode(const NodeBufferWrite &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    if (source.buffers.at(node.bufferName).dynamic) {
        if (node.index == nullptr) {
            throw std::runtime_error("Buffer write on dynamic buffer with no index");
        }
        return prefix
               + bufferPrefix
               + node.bufferName
               + "."
               + bufferArrayName
               + "["
               + compileNode(*node.index, source, functionName)
               + "]."
               + node.elementName
               + " = "
               + compileNode(*node.value, source, functionName);
    } else {
        return prefix
               + bufferPrefix
               + node.bufferName
               + "."
               + bufferArrayName
               + "[gl_DrawID]."
               + node.elementName
               + " = "
               + compileNode(*node.value, source, functionName);
    }
}

std::string compileLeafNode(const NodeBufferSize &node,
                            const Shader &source) {
    if (!source.buffers.at(node.bufferName).dynamic) {
        throw std::runtime_error("Buffer size read on non-dynamic buffer");
    }
    return bufferPrefix + node.bufferName + "." + bufferArrayName + ".length()";
}

std::string compileLeafNode(const NodeAdd &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " + "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeSubtract &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " - "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeMultiply &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " * "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeDivide &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " / "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeEqual &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " == "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeNotEqual &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " != "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeGreater &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " > "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeLess &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " < "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeGreaterEqual &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " >= "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeLessEqual &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " <= "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeAnd &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " && "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeOr &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " || "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const NodeCall &node,
                            const Shader &source,
                            const std::string &functionName) {
    auto ret = node.functionName + "(";
    size_t argCount = 0;
    for (auto &arg: node.arguments) {
        if (argCount > 0) {
            ret += ", ";
        }
        ret += compileNode(*arg, source, functionName, "");
        argCount++;
    }
    ret += ")";
    return ret;
}

std::string compileLeafNode(const NodeReturn &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    return prefix + "return " + compileNode(*node.value, source, functionName);
}

std::string compileLeafNode(const NodeBuiltin &node,
                            const Shader &source,
                            const std::string &functionName) {
    switch (node.type) {
        case NodeBuiltin::ABS:
            return "abs(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::SIN:
            return "sin(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::COS:
            return "cos(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::TAN:
            return "tan(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::ASIN:
            return "asin(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::ACOS:
            return "acos(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::ATAN:
            return "atan(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::POW:
            return "pow(" + compileNode(*node.valA, source, functionName) + ", " + compileNode(
                       *node.valB, source, functionName) + ")";
        case NodeBuiltin::EXP:
            return "exp(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::LOG:
            return "log(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::SQRT:
            return "sqrt(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::INVERSESQRT:
            return "inversesqrt(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::FLOOR:
            return "floor(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::CEIL:
            return "ceil(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::ROUND:
            return "round(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::FRACT:
            return "fract(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::MOD:
            return "mod(" + compileNode(*node.valA, source, functionName) + ", " + compileNode(
                       *node.valB, source, functionName) + ")";
        case NodeBuiltin::MIN:
            return "min(" + compileNode(*node.valA, source, functionName) + ", " + compileNode(
                       *node.valB, source, functionName) + ")";
        case NodeBuiltin::MAX:
            return "max(" + compileNode(*node.valA, source, functionName) + ", " + compileNode(
                       *node.valB, source, functionName) + ")";
        case NodeBuiltin::CLAMP:
            return "clamp(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ", " + compileNode(*node.valC, source, functionName)
                   + ")";
        case NodeBuiltin::MIX:
            return "mix(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ", " + compileNode(*node.valC, source, functionName)
                   + ")";
        case NodeBuiltin::STEP:
            return "step(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ")";
        case NodeBuiltin::SMOOTHSTEP:
            return "smoothstep(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ", " + compileNode(*node.valC, source, functionName)
                   + ")";
        case NodeBuiltin::DOT:
            return "dot(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ")";
        case NodeBuiltin::CROSS:
            return "cross(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ")";
        case NodeBuiltin::NORMALIZE:
            return "normalize(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::LENGTH:
            return "length(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::DISTANCE:
            return "distance(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ")";
        case NodeBuiltin::REFLECT:
            return "reflect(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ")";
        case NodeBuiltin::REFRACT:
            return "refract(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ", " + compileNode(*node.valC, source, functionName)
                   + ")";
        case NodeBuiltin::FACEFORWARD:
            return "faceforward(" + compileNode(*node.valA, source, functionName)
                   + ", " + compileNode(*node.valB, source, functionName)
                   + ", " + compileNode(*node.valC, source, functionName)
                   + ")";
        case NodeBuiltin::TRANSPOSE:
            return "transpose(" + compileNode(*node.valA, source, functionName) + ")";
        case NodeBuiltin::INVERSE:
            return "inverse(" + compileNode(*node.valA, source, functionName) + ")";
        default:
            throw std::runtime_error("Invalid builtin type");
    }
}

std::string compileLeafNode(const NodeSubscriptArray &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.array, source, functionName)
           + "["
           + compileNode(*node.index, source, functionName)
           + "]";
}

std::string compileLeafNode(const NodeVectorSwizzle &node,
                            const Shader &source,
                            const std::string &functionName) {
    if (node.indices.size() < 1 || node.indices.size() > 4) {
        throw std::runtime_error("Invalid vector subscript indices size");
    }
    std::string ret = compileNode(*node.vector, source, functionName) + ".";
    for (auto &index: node.indices) {
        switch (index) {
            case NodeVectorSwizzle::COMPONENT_X:
                ret += "x";
                break;
            case NodeVectorSwizzle::COMPONENT_Y:
                ret += "y";
                break;
            case NodeVectorSwizzle::COMPONENT_Z:
                ret += "z";
                break;
            case NodeVectorSwizzle::COMPONENT_W:
                ret += "w";
                break;
            default:
                throw std::runtime_error("Invalid vector subscript index");
        }
    }
    return ret;
}

std::string compileLeafNode(const NodeSubscriptMatrix &node,
                            const Shader &source,
                            const std::string &functionName) {
    return compileNode(*node.matrix, source, functionName)
           + "["
           + compileNode(*node.column, source, functionName)
           + "]["
           + compileNode(*node.row, source, functionName)
           + "]";
}

std::string compileLeafNode(const NodeBranch &node,
                            const Shader &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    std::string ret;
    ret += prefix + "if (";
    ret += compileNode(*node.condition, source, functionName);
    ret += ") {\n";
    for (auto &branchNode: node.trueBranch) {
        ret += compileNode(*branchNode, source, functionName, prefix + "\t");
        ret += ";\n";
    }
    ret += prefix + "}";
    if (node.falseBranch.size() > 0) {
        ret += " else {\n";
        for (auto &branchNode: node.falseBranch) {
            ret += compileNode(*branchNode, source, functionName, prefix + "\t");
            ret += ";\n";
        }
        ret += prefix + "}";
    }
    return ret;
}

std::string compileLeafNode(const NodeLoop &node,
                            const Shader &source,
                            const std::string &functionName, const std::string &prefix) {
    std::string ret;
    ret += prefix + "for (";
    if (node.initializer != nullptr) {
        ret += compileNode(*node.initializer, source, functionName);
    }
    ret += "; ";
    if (node.predicate != nullptr) {
        ret += compileNode(*node.predicate, source, functionName);
    }
    ret += "; ";
    if (node.iterator != nullptr) {
        ret += compileNode(*node.iterator, source, functionName);
    }
    ret += ") {\n";
    for (auto &branchNode: node.body) {
        ret += compileNode(*branchNode, source, functionName, prefix + "\t");
        ret += ";\n";
    }
    ret += prefix + "}";
    return ret;
}

std::string compileLeafNode(const NodeVertexPosition &node, const Shader &source, const std::string &functionName,
                            const std::string &prefix) {
    return prefix + "gl_Position = " + compileNode(*node.value, source, functionName);
}
