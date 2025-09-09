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
            break;
        case FGShaderNode::TEXTURE_SAMPLE:
            break;
        case FGShaderNode::TEXTURE_SIZE:
            break;
        case FGShaderNode::BUFFER_READ:
            return createCompiledNode(down_cast<FGNodeBufferRead &>(*node), node, source);
        case FGShaderNode::BUFFER_WRITE:
            break;
        case FGShaderNode::BUFFER_SIZE:
            break;
        case FGShaderNode::ADD:
            break;
        case FGShaderNode::SUBTRACT:
            break;
        case FGShaderNode::MULTIPLY:
            return createCompiledNode(down_cast<FGNodeMultiply &>(*node), node);
        case FGShaderNode::DIVIDE:
            break;
        case FGShaderNode::EQUAL:
            break;
        case FGShaderNode::NEQUAL:
            break;
        case FGShaderNode::GREATER:
            break;
        case FGShaderNode::LESS:
            break;
        case FGShaderNode::GREATER_EQUAL:
            break;
        case FGShaderNode::LESS_EQUAL:
            break;
        case FGShaderNode::AND:
            break;
        case FGShaderNode::OR:
            break;
        case FGShaderNode::NORMALIZE:
            break;
        case FGShaderNode::SUBSCRIPT:
            return createCompiledNode(down_cast<FGNodeSubscript &>(*node), node, source);
        case FGShaderNode::BRANCH:
            break;
        case FGShaderNode::LOOP:
            break;
    }
    throw std::runtime_error("Node Type not implemented");
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

CompiledNode createCompiledNode(const FGNodeLiteral &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(literalToString(node.value));
    return ret;
}

CompiledNode createCompiledNode(const FGNodeAdd &node, const std::shared_ptr<FGShaderNode> &nodePtr) {
    CompiledNode ret(nodePtr);
    ret.content.emplace_back(node.left.source);
    ret.content.emplace_back(" + ");
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
