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

std::string compileNode(const FGShaderNode &node,
                        const FGShaderSource &source,
                        const std::string &functionName,
                        const std::string &prefix) {
    switch (node.getType()) {
        case FGShaderNode::VARIABLE_CREATE:
            return compileLeafNode(down_cast<const FGNodeVariableCreate &>(node), source, functionName, prefix);
        case FGShaderNode::VARIABLE_WRITE:
            return compileLeafNode(down_cast<const FGNodeVariableWrite &>(node), source, functionName, prefix);
        case FGShaderNode::VARIABLE_READ:
            return compileLeafNode(down_cast<const FGNodeVariableRead &>(node));
        case FGShaderNode::LITERAL:
            return compileLeafNode(down_cast<const FGNodeLiteral &>(node));
        case FGShaderNode::ARGUMENT:
            return compileLeafNode(down_cast<const FGNodeArgument &>(node));
        case FGShaderNode::VECTOR:
            return compileLeafNode(down_cast<const FGNodeVector &>(node), source, functionName);
        case FGShaderNode::ATTRIBUTE_READ:
            return compileLeafNode(down_cast<const FGNodeAttributeRead &>(node));
        case FGShaderNode::ATTRIBUTE_WRITE:
            return compileLeafNode(down_cast<const FGNodeAttributeWrite &>(node), source, functionName, prefix);
        case FGShaderNode::PARAMETER_READ:
            return compileLeafNode(down_cast<const FGNodeParameterRead &>(node));
        case FGShaderNode::TEXTURE_SAMPLE:
            return compileLeafNode(down_cast<const FGNodeTextureSample &>(node), source, functionName);
        case FGShaderNode::TEXTURE_SIZE:
            return compileLeafNode(down_cast<const FGNodeTextureSize &>(node));
        case FGShaderNode::BUFFER_READ:
            return compileLeafNode(down_cast<const FGNodeBufferRead &>(node), source, functionName);
        case FGShaderNode::BUFFER_WRITE:
            return compileLeafNode(down_cast<const FGNodeBufferWrite &>(node), source, functionName, prefix);
        case FGShaderNode::BUFFER_SIZE:
            return compileLeafNode(down_cast<const FGNodeBufferSize &>(node), source);
        case FGShaderNode::ADD:
            return compileLeafNode(down_cast<const FGNodeAdd &>(node), source, functionName);
        case FGShaderNode::SUBTRACT:
            return compileLeafNode(down_cast<const FGNodeSubtract &>(node), source, functionName);
        case FGShaderNode::MULTIPLY:
            return compileLeafNode(down_cast<const FGNodeMultiply &>(node), source, functionName);
        case FGShaderNode::DIVIDE:
            return compileLeafNode(down_cast<const FGNodeDivide &>(node), source, functionName);
        case FGShaderNode::EQUAL:
            return compileLeafNode(down_cast<const FGNodeEqual &>(node), source, functionName);
        case FGShaderNode::NEQUAL:
            return compileLeafNode(down_cast<const FGNodeNotEqual &>(node), source, functionName);
        case FGShaderNode::GREATER:
            return compileLeafNode(down_cast<const FGNodeGreater &>(node), source, functionName);
        case FGShaderNode::LESS:
            return compileLeafNode(down_cast<const FGNodeLess &>(node), source, functionName);
        case FGShaderNode::GREATER_EQUAL:
            return compileLeafNode(down_cast<const FGNodeGreaterEqual &>(node), source, functionName);
        case FGShaderNode::LESS_EQUAL:
            return compileLeafNode(down_cast<const FGNodeLessEqual &>(node), source, functionName);
        case FGShaderNode::AND:
            return compileLeafNode(down_cast<const FGNodeAnd &>(node), source, functionName);
        case FGShaderNode::OR:
            return compileLeafNode(down_cast<const FGNodeOr &>(node), source, functionName);
        case FGShaderNode::CALL:
            return compileLeafNode(down_cast<const FGNodeCall &>(node), source, functionName);
        case FGShaderNode::RETURN:
            return compileLeafNode(down_cast<const FGNodeReturn &>(node), source, functionName);
        case FGShaderNode::NORMALIZE:
            return compileLeafNode(down_cast<const FGNodeNormalize &>(node), source, functionName);
        case FGShaderNode::SUBSCRIPT_ARRAY:
            return compileLeafNode(down_cast<const FGNodeSubscriptArray &>(node), source, functionName);
        case FGShaderNode::SUBSCRIPT_VECTOR:
            return compileLeafNode(down_cast<const FGNodeSubscriptVector &>(node), source, functionName);
        case FGShaderNode::SUBSCRIPT_MATRIX:
            return compileLeafNode(down_cast<const FGNodeSubscriptMatrix &>(node), source, functionName);
        case FGShaderNode::BRANCH:
            return compileLeafNode(down_cast<const FGNodeBranch &>(node), source, functionName, prefix);
        case FGShaderNode::LOOP:
            return compileLeafNode(down_cast<const FGNodeLoop &>(node), source, functionName, prefix);
    }
    throw std::runtime_error("Node Type not implemented");
}

std::string compileLeafNode(const FGNodeVariableCreate &node,
                            const FGShaderSource &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    auto ret = getTypeName(node.type) + " " + node.variableName;
    if (node.value != nullptr) {
        ret += " = " + compileNode(*node.value, source, functionName);
    }
    return prefix + ret;
}

std::string compileLeafNode(const FGNodeVariableWrite &node,
                            const FGShaderSource &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    return prefix + node.variableName + " = " + compileNode(*node.value, source, functionName);
}

std::string compileLeafNode(const FGNodeVariableRead &node) {
    return node.variableName;
}

std::string compileLeafNode(const FGNodeLiteral &node) {
    return literalToString(node.value);
}

std::string compileLeafNode(const FGNodeArgument &node) {
    return node.argumentName;
}

std::string compileLeafNode(const FGNodeVector &node,
                            const FGShaderSource &source,
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

std::string compileLeafNode(const FGNodeAttributeRead &node) {
    return inputAttributePrefix + std::to_string(node.attributeIndex);
}

std::string compileLeafNode(const FGNodeAttributeWrite &node,
                            const FGShaderSource &source,
                            const std::string &functionName,
                            const std::string &prefix) {
    return prefix
           + outputAttributePrefix
           + std::to_string(node.attributeIndex)
           + " = "
           + compileNode(*node.value, source, functionName);
}

std::string compileLeafNode(const FGNodeParameterRead &node) {
    throw std::runtime_error("Parameter read not implemented");
}

std::string compileLeafNode(const FGNodeTextureSample &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    auto ret = "texture("
               + std::string(texturePrefix)
               + node.textureName
               + ", "
               + compileNode(*node.coordinate, source, functionName, "");
    if (node.bias != nullptr) {
        ret += ", " + compileNode(*node.bias, source, functionName, "");
    }
    ret += ")";
    return ret;
}

std::string compileLeafNode(const FGNodeTextureSize &node) {
    return "textureSize(" + std::string(texturePrefix) + node.textureName + ")";
}

std::string compileLeafNode(const FGNodeBufferRead &node,
                            const FGShaderSource &source,
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
               + "[gl_DrawID]."
               + node.elementName;
    }
}

std::string compileLeafNode(const FGNodeBufferWrite &node,
                            const FGShaderSource &source,
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

std::string compileLeafNode(const FGNodeBufferSize &node,
                            const FGShaderSource &source) {
    if (!source.buffers.at(node.bufferName).dynamic) {
        throw std::runtime_error("Buffer size read on non-dynamic buffer");
    }
    return bufferPrefix + node.bufferName + "." + bufferArrayName + ".length()";
}

std::string compileLeafNode(const FGNodeAdd &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " + "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeSubtract &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " - "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeMultiply &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " * "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeDivide &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " / "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeEqual &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " == "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeNotEqual &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " != "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeGreater &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " > "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeLess &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " < "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeGreaterEqual &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " >= "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeLessEqual &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " <= "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeAnd &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " && "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeOr &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.left, source, functionName)
           + " || "
           + compileNode(*node.right, source, functionName);
}

std::string compileLeafNode(const FGNodeCall &node,
                            const FGShaderSource &source,
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

std::string compileLeafNode(const FGNodeReturn &node, const FGShaderSource &source, const std::string &functionName) {
    return "return " + compileNode(*node.value, source, functionName);
}

std::string compileLeafNode(const FGNodeNormalize &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return "normalize(" + compileNode(*node.value, source, functionName) + ")";
}

std::string compileLeafNode(const FGNodeSubscriptArray &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.array, source, functionName)
           + "["
           + compileNode(*node.index, source, functionName)
           + "]";
}

std::string compileLeafNode(const FGNodeSubscriptVector &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    switch (node.index) {
        case 0:
            return compileNode(*node.vector, source, functionName) + ".x";
        case 1:
            return compileNode(*node.vector, source, functionName) + ".y";
        case 2:
            return compileNode(*node.vector, source, functionName) + ".z";
        case 3:
            return compileNode(*node.vector, source, functionName) + ".w";
        default:
            throw std::runtime_error("Invalid vector subscript index");
    }
}

std::string compileLeafNode(const FGNodeSubscriptMatrix &node,
                            const FGShaderSource &source,
                            const std::string &functionName) {
    return compileNode(*node.matrix, source, functionName)
           + "["
           + compileNode(*node.column, source, functionName)
           + "]["
           + compileNode(*node.row, source, functionName)
           + "]";
}

std::string compileLeafNode(const FGNodeBranch &node,
                            const FGShaderSource &source,
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

std::string compileLeafNode(const FGNodeLoop &node,
                            const FGShaderSource &source,
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
