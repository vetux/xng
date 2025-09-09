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

CompiledNode createCompiledNode(std::shared_ptr<FGShaderNode> node, const FGShaderSource &source) {
    switch (node->getType()) {
        case FGShaderNode::LITERAL:
            return createCompiledNode(down_cast<FGNodeLiteral &>(*node), node);
        case FGShaderNode::VECTOR:
            return createCompiledNode(down_cast<FGNodeVector &>(*node), node, source);
        case FGShaderNode::ATTRIBUTE_READ:
            return createCompiledNode(down_cast<FGNodeAttributeRead &>(*node), node);
        case FGShaderNode::ATTRIBUTE_WRITE:
            return createCompiledNode(down_cast<FGNodeAttributeWrite &>(*node), node);
        case FGShaderNode::PARAMETER_READ:
            return createCompiledNode(down_cast<FGNodeParameterRead &>(*node), node);
        case FGShaderNode::TEXTURE_SAMPLE:
            return createCompiledNode(down_cast<FGNodeTextureSample &>(*node), node, source);
        case FGShaderNode::TEXTURE_SIZE:
            return createCompiledNode(down_cast<FGNodeTextureSize &>(*node), node);
        case FGShaderNode::BUFFER_READ:
            return createCompiledNode(down_cast<FGNodeBufferRead &>(*node), node, source);
        case FGShaderNode::BUFFER_WRITE:
            return createCompiledNode(down_cast<FGNodeBufferWrite &>(*node), node, source);
        case FGShaderNode::BUFFER_SIZE:
            return createCompiledNode(down_cast<FGNodeBufferSize &>(*node), node, source);
        case FGShaderNode::ADD:
            return createCompiledNode(down_cast<FGNodeAdd &>(*node), node);
        case FGShaderNode::SUBTRACT:
            return createCompiledNode(down_cast<FGNodeSubtract &>(*node), node);
        case FGShaderNode::MULTIPLY:
            return createCompiledNode(down_cast<FGNodeMultiply &>(*node), node);
        case FGShaderNode::DIVIDE:
            return createCompiledNode(down_cast<FGNodeDivide &>(*node), node);
        case FGShaderNode::EQUAL:
            return createCompiledNode(down_cast<FGNodeEqual &>(*node), node);
        case FGShaderNode::NEQUAL:
            return createCompiledNode(down_cast<FGNodeNotEqual &>(*node), node);
        case FGShaderNode::GREATER:
            return createCompiledNode(down_cast<FGNodeGreater &>(*node), node);
        case FGShaderNode::LESS:
            return createCompiledNode(down_cast<FGNodeLess &>(*node), node);
        case FGShaderNode::GREATER_EQUAL:
            return createCompiledNode(down_cast<FGNodeGreaterEqual &>(*node), node);
        case FGShaderNode::LESS_EQUAL:
            return createCompiledNode(down_cast<FGNodeLessEqual &>(*node), node);
        case FGShaderNode::AND:
            return createCompiledNode(down_cast<FGNodeAnd &>(*node), node);
        case FGShaderNode::OR:
            return createCompiledNode(down_cast<FGNodeOr &>(*node), node);
        case FGShaderNode::NORMALIZE:
            return createCompiledNode(down_cast<FGNodeNormalize &>(*node), node);
        case FGShaderNode::SUBSCRIPT:
            return createCompiledNode(down_cast<FGNodeSubscript &>(*node), node, source);
        case FGShaderNode::BRANCH:
            return createCompiledNode(down_cast<FGNodeBranch &>(*node), node);
        case FGShaderNode::LOOP:
            return createCompiledNode(down_cast<FGNodeLoop &>(*node), node);
    }
    throw std::runtime_error("Node Type not implemented");
}

CompiledNode createCompiledNode(const FGNodeLiteral &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(literalToString(node.value));
    return ret;
}

CompiledNode createCompiledNode(const FGNodeVector &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(getTypeName(node.getOutputType(source)));
    ret.content.emplace_back("(");
    ret.content.emplace_back(node.x.source);
    ret.content.emplace_back(", ");
    ret.content.emplace_back(node.y.source);
    if (node.z.source != nullptr) {
        ret.content.emplace_back(", ");
        ret.content.emplace_back(node.z.source);
        if (node.w.source != nullptr) {
            ret.content.emplace_back(", ");
            ret.content.emplace_back(node.w.source);
        }
    }
    ret.content.emplace_back(")");
    return ret;
}

CompiledNode createCompiledNode(const FGNodeAttributeRead &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(inputAttributePrefix + std::to_string(node.attributeIndex));
    return ret;
}

CompiledNode createCompiledNode(const FGNodeAttributeWrite &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(outputAttributePrefix + std::to_string(node.attributeIndex) + " = ");
    ret.content.emplace_back(node.value.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeParameterRead &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    throw std::runtime_error("Parameter read not implemented");
}

CompiledNode createCompiledNode(const FGNodeTextureSample &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back("texture(");
    ret.content.emplace_back(texturePrefix + node.textureName);
    ret.content.emplace_back(", ");
    ret.content.emplace_back(node.coordinate.source);
    if (node.bias.source != nullptr) {
        ret.content.emplace_back(", ");
        ret.content.emplace_back(node.bias.source);
    }
    ret.content.emplace_back(")");
    return ret;
}

CompiledNode createCompiledNode(const FGNodeTextureSize &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back("textureSize(");
    ret.content.emplace_back(texturePrefix + node.textureName);
    ret.content.emplace_back(")");
    return ret;
}

CompiledNode createCompiledNode(const FGNodeBufferRead &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source) {
    CompiledNode ret(nodePtr);
    if (source.buffers.at(node.bufferName).dynamic) {
        ret.content.emplace_back(bufferPrefix
                                 + node.bufferName
                                 + "."
                                 + bufferArrayName
                                 + "[");
        if (node.index.source == nullptr) {
            throw std::runtime_error("Dynamic buffer read with no index");
        }
        ret.content.emplace_back(node.index.source);
        ret.content.emplace_back("]." + node.elementName);
    } else {
        ret.content.emplace_back(bufferPrefix
                                 + node.bufferName
                                 + "."
                                 + bufferArrayName
                                 + "[gl_DrawID]."
                                 + node.elementName);
    }
    return ret;
}

CompiledNode createCompiledNode(const FGNodeBufferWrite &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source) {
    CompiledNode ret(nodePtr);

    if (auto buffer = source.buffers.at(node.bufferName); buffer.dynamic) {
        ret.content.emplace_back(bufferPrefix + node.bufferName + "." + bufferArrayName + "[");
        ret.content.emplace_back(node.index.source);
        ret.content.emplace_back("]." + node.elementName + " = ");
        ret.content.emplace_back(node.value.source);
    } else {
        ret.content.emplace_back(bufferPrefix
                                 + node.bufferName
                                 + "."
                                 + bufferArrayName
                                 + "[gl_DrawID]."
                                 + node.elementName
                                 + " = ");
        ret.content.emplace_back(node.value.source);
    }

    return ret;
}

CompiledNode createCompiledNode(const FGNodeBufferSize &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source) {
    if (!source.buffers.at(node.bufferName).dynamic) {
        throw std::runtime_error("Buffer size read on non-dynamic buffer");
    }
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(bufferPrefix + node.bufferName + "." + bufferArrayName + ".length()");
    return ret;
}

CompiledNode createCompiledNode(const FGNodeAdd &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" + ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeSubtract &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" - ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeMultiply &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" * ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeDivide &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" / ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeEqual &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" == ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeNotEqual &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" != ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeGreater &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" > ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeLess &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" < ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeGreaterEqual &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" >= ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeLessEqual &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" <= ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeAnd &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" && ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeOr &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" || ");
    ret.content.emplace_back(node.right.source);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeNormalize &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back("normalize(");
    ret.content.emplace_back(node.value.source);
    ret.content.emplace_back(")");
    return ret;
}

CompiledNode createCompiledNode(const FGNodeSubscript &node,
                                const std::shared_ptr<FGShaderNode> &nodePtr,
                                const FGShaderSource &source) {
    CompiledNode ret(nodePtr);
    auto valueType = node.value.source->getOutputType(source);
    if (valueType.count > 1) {
        // Array subscripting
        ret.content.emplace_back(node.value.source);
        ret.content.emplace_back("[");
        ret.content.emplace_back(node.row.source);
        ret.content.emplace_back("]");
    } else if (valueType.type == FGShaderValue::MAT2
               || valueType.type == FGShaderValue::MAT3
               || valueType.type == FGShaderValue::MAT4) {
        // Matrix subscripting
        ret.content.emplace_back(node.value.source);
        ret.content.emplace_back("[");
        ret.content.emplace_back(node.column.source);
        ret.content.emplace_back("][");
        ret.content.emplace_back(node.row.source);
        ret.content.emplace_back("]");
    } else {
        // Vector subscripting
        ret.content.emplace_back(node.value.source);

        auto indexSource = node.row.source;
        if (indexSource->getType() != FGShaderNode::LITERAL) {
            throw std::runtime_error("Subscripting vectors with non-literal index is not supported");
        }
        auto index = std::get<int>(down_cast<FGNodeLiteral &>(*indexSource).value);
        switch (index) {
            case 0:
                ret.content.emplace_back(".x");
                break;
            case 1:
                ret.content.emplace_back(".y");
                break;
            case 2:
                ret.content.emplace_back(".z");
                break;
            case 3:
                ret.content.emplace_back(".w");
                break;
            default:
                throw std::runtime_error("Invalid vector subscript index");
        }
    }
    return ret;
}

CompiledNode createCompiledNode(const FGNodeBranch &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    return ret;
}

CompiledNode createCompiledNode(const FGNodeLoop &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    return ret;
}
