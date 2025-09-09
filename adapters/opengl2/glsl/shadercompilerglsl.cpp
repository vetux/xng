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
#include "xng/util/downcast.hpp"

using namespace xng;

CompiledTree createCompiledTree(const FGShaderSource &source) {
    CompiledTree ret;
    size_t varCount = 0;
    for (const auto &node: source.nodes) {
        ret.nodes[node] = createCompiledNode(node, source);
        if ((node->getOutputs().size() > 0 && node->getOutput().consumers.size() > 1)
            || node->getType() == FGShaderNode::BRANCH) {
            ret.variables[node] = "tmp" + std::to_string(varCount++);
        }
    }
    return ret;
}

std::string getValueCode(const CompiledNode &node, const CompiledTree &tree) {
    std::string ret;
    if (node.node->getType() == FGShaderNode::BRANCH) {
        ret = tree.variables.at(node.node);
    } else {
        for (auto var: node.content) {
            if (var.index() == 0) {
                ret += std::get<std::string>(var);
            } else {
                auto sourceNode = std::get<std::shared_ptr<FGShaderNode> >(var);
                if (tree.variables.find(sourceNode) == tree.variables.end()) {
                    // Inline
                    ret += getValueCode(tree.nodes.at(sourceNode), tree);
                } else {
                    // Use Variable
                    ret += tree.variables.at(sourceNode);
                }
            }
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
    if (node.node->getInputs().size() > 0) {
        for (const auto &input: node.node->getInputs()) {
            if (input.get().source != nullptr) {
                ret += compileVariables(tree.nodes.at(input.get().source), tree, source, visited, prefix);
            }
        }
    }

    auto var = tree.variables.find(node.node);
    if (var != tree.variables.end()
        && visited.find(node.node) == visited.end()) {
        if (node.node->getType() == FGShaderNode::BRANCH) {
            auto branchNode = down_cast<FGNodeBranch &>(*node.node);

            auto condition = tree.nodes.at(branchNode.condition.source);
            auto trueBranch = tree.nodes.at(branchNode.trueBranch.source);
            auto falseBranch = tree.nodes.at(branchNode.falseBranch.source);

            ret += prefix + getTypeName(node.node->getOutputType(source)) + " " + var->second + ";\n";
            ret += prefix + "if (" + getValueCode(condition, tree) + ") {\n";
            if (tree.variables.find(trueBranch.node) == tree.variables.end()) {
                ret += prefix
                        + "\t"
                        + var->second
                        + " = "
                        + getValueCode(tree.nodes.at(branchNode.trueBranch.source), tree)
                        + ";\n";
            } else {
                ret += prefix + "\t" + var->second + " = " + tree.variables.at(trueBranch.node) + ";\n";
            }
            ret += prefix + "} else {\n";
            if (tree.variables.find(falseBranch.node) == tree.variables.end()) {
                ret += prefix + "\t" + var->second + " = " + getValueCode(tree.nodes.at(branchNode.falseBranch.source),
                                                                          tree) + ";\n";
            } else {
                ret += prefix + "\t" + var->second + " = " + tree.variables.at(falseBranch.node) + ";\n";
            }
            ret += prefix + "}\n";
        } else {
            // This node has multiple consumers, assign it to a variable
            ret += prefix
                    + getTypeName(node.node->getOutputType(source))
                    + " "
                    + var->second
                    + " = "
                    + getValueCode(node, tree)
                    + ";\n";
        }
        visited.insert(node.node);
    }
    return ret;
}

std::string compileTree(const CompiledTree &tree, const FGShaderSource &source, std::string prefix = "\t") {
    std::vector<CompiledNode> baseNodes;
    for (const auto &node: tree.nodes) {
        if (node.first->getOutputs().empty()) {
            baseNodes.emplace_back(node.second);
        }
    }

    // Generate variables
    std::unordered_set<std::shared_ptr<FGShaderNode> > visited;
    std::string ret;
    for (auto &node: baseNodes) {
        ret += compileVariables(node, tree, source, visited, prefix);
    }

    // Generate output
    for (const auto &node: baseNodes) {
        ret += prefix + getValueCode(node, tree) + ";\n";
    }

    return ret;
}

std::string getSampler(const FGTexture &texture) {
    std::string prefix;
    if (texture.format >= R8I && texture.format <= RGBA32I) {
        prefix = "i";
    } else if (texture.format >= R8UI && texture.format <= RGBA32UI) {
        prefix = "u";
    }

    if (texture.arrayLayers > 1) {
        switch (texture.textureType) {
            case TEXTURE_2D:
                return prefix + "sampler2DArray";
            case TEXTURE_2D_MULTISAMPLE:
                return prefix + "sampler2DMSArray";
            case TEXTURE_CUBE_MAP:
                return prefix + "samplerCubeArray";
            default:
                throw std::runtime_error("Unrecognized texture type");
        }
    } else {
        switch (texture.textureType) {
            case TEXTURE_2D:
                return prefix + "sampler2D";
            case TEXTURE_2D_MULTISAMPLE:
                return prefix + "sampler2DMS";
            case TEXTURE_CUBE_MAP:
                return prefix + "samplerCube";
            default:
                throw std::runtime_error("Unrecognized texture type");
        }
    }
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

std::string generateHeader(const FGShaderSource &source, CompiledPipeline &pipeline) {
    std::string ret;

    for (const auto &pair: source.buffers) {
        auto binding = pipeline.getBufferBinding(pair.first);

        std::string bufferLayout = "struct ShaderBufferData" + std::to_string(binding) + " {\n";
        for (const auto &element: pair.second.elements) {
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

    for (const auto &pair: source.textures) {
        const auto location = pipeline.getTextureBinding(pair.first);
        ret += "layout(binding = "
                + std::to_string(location)
                + ") uniform "
                + getSampler(pair.second)
                + " "
                + texturePrefix
                + pair.first
                + ";\n";
    }

    if (source.textures.size() > 0) {
        ret += "\n";
    }

    std::string inputAttributes;
    size_t attributeCount = 0;
    for (auto element: source.inputLayout.elements) {
        auto location = attributeCount++;
        inputAttributes += "layout(location = "
                + std::to_string(location)
                + ") in "
                + generateElement(inputAttributePrefix + std::to_string(location), element, "");
    }
    ret += inputAttributes;
    ret += "\n";

    std::string outputAttributes;
    attributeCount = 0;
    for (auto element: source.outputLayout.elements) {
        auto location = attributeCount++;
        outputAttributes += "layout(location = "
                + std::to_string(location)
                + ") out "
                + generateElement(outputAttributePrefix + std::to_string(location), element, "");
    }
    ret += outputAttributes;
    ret += "\n";

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
    for (auto &shader: sources) {
        ret.sourceCode[shader.stage] = compileShader(shader, ret);
    }
    return ret;
}
