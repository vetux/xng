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

#include "shadercompilerglsl.hpp"

#include "xng/render/graph2/shader/fgshadernode.hpp"

using namespace xng;

static std::string debugNodeTypeName(FGShaderNode::NodeType type) {
    switch (type) {
        case FGShaderNode::LITERAL:
            return "LITERAL";
        case FGShaderNode::VECTOR:
            return "VECTOR";
        case FGShaderNode::ATTRIBUTE_READ:
            return "ATTRIBUTE_READ";
        case FGShaderNode::ATTRIBUTE_WRITE:
            return "ATTRIBUTE_WRITE";
        case FGShaderNode::PARAMETER_READ:
            return "PARAMETER_READ";
        case FGShaderNode::TEXTURE_SAMPLE:
            return "TEXTURE_SAMPLE";
        case FGShaderNode::TEXTURE_SIZE:
            return "TEXTURE_SIZE";
        case FGShaderNode::BUFFER_READ:
            return "BUFFER_READ";
        case FGShaderNode::BUFFER_WRITE:
            return "BUFFER_WRITE";
        case FGShaderNode::ADD:
            return "ADD";
        case FGShaderNode::SUBTRACT:
            return "SUBTRACT";
        case FGShaderNode::MULTIPLY:
            return "MULTIPLY";
        case FGShaderNode::DIVIDE:
            return "DIVIDE";
        case FGShaderNode::EQUAL:
            return "EQUAL";
        case FGShaderNode::NEQUAL:
            return "NEQUAL";
        case FGShaderNode::GREATER:
            return "GREATER";
        case FGShaderNode::LESS:
            return "LESS";
        case FGShaderNode::GREATER_EQUAL:
            return "GREATER_EQUAL";
        case FGShaderNode::LESS_EQUAL:
            return "LESS_EQUAL";
        case FGShaderNode::AND:
            return "AND";
        case FGShaderNode::OR:
            return "OR";
        case FGShaderNode::NORMALIZE:
            return "NORMALIZE";
        case FGShaderNode::ARRAY_LENGTH:
            return "ARRAY_LENGTH";
        case FGShaderNode::SUBSCRIPT:
            return "SUBSCRIPT";
        case FGShaderNode::BRANCH:
            return "BRANCH";
        case FGShaderNode::LOOP:
            return "LOOP";
    }
}

static std::string debugTraverseTreePrefix(size_t depth = 0) {
    std::string ret;
    if (depth > 0) {
        for (size_t i = 0; i <= depth; i++) {
            ret += " ";
        }
        ret += "|-";
    }
    return ret;
}

static std::string debugTraverseTree(FGShaderNode &node, size_t depth = 0) {
    std::string ret = debugTraverseTreePrefix(depth) + debugNodeTypeName(node.getType()) + "\n";

    for (auto input: node.getInputs()) {
        ret += debugTraverseTreePrefix(depth + 2) + input.get().name + "\n";
        if (input.get().source != nullptr) {
            ret += debugTraverseTree(*input.get().source, depth + 4);
        }
    }
    return ret;
}

static std::string debugNodeTree(const std::vector<std::shared_ptr<FGShaderNode> > nodes) {
    std::vector<std::shared_ptr<FGShaderNode> > baseNodes;
    for (auto node: nodes) {
        if (node->getOutputs().empty()) {
            baseNodes.emplace_back(node);
        }
    }

    std::string ret;
    for (auto node: baseNodes) {
        ret = ret + debugTraverseTree(*node);
        ret += "\n";
    }
    return ret;
}

std::string ShaderCompilerGLSL::compile(const FGShaderSource &source) {
    return debugNodeTree(source.nodes);
}
