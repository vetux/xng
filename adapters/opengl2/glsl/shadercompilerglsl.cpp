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

#include "literals.hpp"
#include "types.hpp"
#include "nodecompiler.hpp"

using namespace xng;

CompiledTree createCompiledTree(const FGShaderSource &source) {
    CompiledTree ret;
    size_t varCount = 0;
    for (const auto &node: source.nodes) {
        ret.nodes[node] = createCompiledNode(node, source);
        if (node->getOutputs().size() > 0
            && node->getOutput().consumers.size() > 1) {
            ret.variables[node] = "tmp" + std::to_string(varCount++);
        }
    }
    return ret;
}

std::string compileVariables(const CompiledNode &node,
                             const CompiledTree &tree,
                             const FGShaderSource &source,
                             std::unordered_set<std::shared_ptr<FGShaderNode> > &visited,
                             std::string prefix) {
    std::string ret;
    auto var = tree.variables.find(node.node);
    if (var != tree.variables.end()
        && visited.find(node.node) == visited.end()) {
        // This node has multiple consumers, replace it with a variable
        ret += prefix
                + getTypeName(node.node->getOutputType(source))
                + " "
                + var->second
                + " = "
                + node.getValueCode(tree)
                + ";\n";
    }
    if (node.node->getOutputs().size() > 0) {
        for (const auto &consumer: node.node->getOutput().consumers) {
            ret += compileVariables(tree.nodes.at(consumer), tree, source, visited, prefix);
        }
    }
    return ret;
}

std::string compileTree(const CompiledTree &tree, const FGShaderSource &source, std::string prefix = "\t") {
    std::vector<CompiledNode> rootNodes;
    for (const auto &node: tree.nodes) {
        if (node.first->getInputs().empty()) {
            rootNodes.emplace_back(node.second);
        }
    }

    // Generate variables
    std::unordered_set<std::shared_ptr<FGShaderNode> > visited;
    std::string ret;
    for (auto &node: rootNodes) {
        ret += compileVariables(node, tree, source, visited, prefix);
    }

    // Generate output
    std::vector<CompiledNode> baseNodes;
    for (const auto &node: tree.nodes) {
        if (node.first->getOutputs().empty()) {
            ret += prefix + node.second.getValueCode(tree) + ";\n";
        }
    }

    return ret;
}

std::string generateElement(const std::string &name, const FGShaderValue &value, std::string prefix = "\t") {
    auto ret = prefix
           + getTypeName(value)
           + " "
           + name;

    if (value.count > 1) {
        ret += "[";
        ret += std::to_string(value.count);
        ret += "]";
    }

    return ret + ";\n";
}

std::string generateHeader(const FGShaderSource &source, CompiledPipeline& pipeline) {
    std::string ret;

    for (const auto& pair: source.buffers) {
        auto binding = pipeline.getBufferBinding(pair.first);

        std::string bufferLayout = "struct ShaderBufferData" + std::to_string(binding) + " {\n";
        for (const auto& element: pair.second.elements) {
            bufferLayout += generateElement(element.name, element.value);
        }
        bufferLayout += "};\n";

        std::string bufferCode = "layout(binding = "
        + std::to_string(binding)
        + ", std140) buffer ShaderBuffer"
        + std::to_string(binding)
        + " {\n"
        + "\tShaderBufferData"
        + std::to_string(binding)
        + " "
        + bufferArrayName
        + "[];\n} "
        + bufferPrefix
        + pair.first
        + ";\n";

        ret += bufferLayout;
        ret += "\n";
        ret += bufferCode;
        ret += "\n";
    }

    for (auto element: source.inputLayout.elements) {
    }
    return ret;
}

std::string compileShader(const FGShaderSource &source, CompiledPipeline &pipeline) {
    auto tree = createCompiledTree(source);
    auto mainBody = compileTree(tree, source);
    return "#version 460\n\n"
           + generateHeader(source, pipeline)
           + "void main() {\n"
           + mainBody
           + "}";
}

CompiledPipeline ShaderCompilerGLSL::compile(const std::vector<FGShaderSource> &sources) {
    CompiledPipeline ret;
    for (auto &shader : sources) {
        ret.sourceCode[shader.stage] = compileShader(shader, ret);
    }
    return ret;
}
