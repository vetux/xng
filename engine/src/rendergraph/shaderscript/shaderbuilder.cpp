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

#include <utility>

#include "xng/rendergraph/shaderscript/shaderbuilder.hpp"

#include "xng/rendergraph/shaderscript/shadernodewrapper.hpp"

namespace xng::ShaderScript {
    thread_local ShaderBuilder fgShaderBuilder;

    ShaderBuilder &ShaderBuilder::instance() {
        return fgShaderBuilder;
    }

    void ShaderBuilder::If(const ShaderNodeWrapper &condition) {
        std::shared_ptr<TreeNode> ifNode = std::make_shared<TreeNode>();
        ifNode->parent = currentNode;
        ifNode->type = TreeNode::IF;
        ifNode->condition = condition.node->copy();
        if (currentNode->processingElse) {
            currentNode->falseBranch.push_back(ifNode);
        } else {
            currentNode->defaultBranch.push_back(ifNode);
        }
        currentNode = ifNode;
    }

    void ShaderBuilder::Else() {
        assert(currentNode->type == TreeNode::IF);
        currentNode->processingElse = true;
    }

    void ShaderBuilder::EndIf() {
        assert(currentNode->type == TreeNode::IF);
        currentNode = currentNode->parent;
    }

    void ShaderBuilder::For(const ShaderNodeWrapper &loopVariable,
                              const ShaderNodeWrapper &initializer,
                              const ShaderNodeWrapper &condition,
                              const ShaderNodeWrapper &incrementor) {
        std::shared_ptr<TreeNode> forNode = std::make_shared<TreeNode>();
        forNode->parent = currentNode;
        forNode->type = TreeNode::FOR;
        forNode->loopVariable = loopVariable.node->copy();
        forNode->condition = condition.node->copy();
        forNode->initializer = initializer.node->copy();
        forNode->incrementor = incrementor.node->copy();
        if (currentNode->processingElse) {
            currentNode->falseBranch.push_back(forNode);
        } else {
            currentNode->defaultBranch.push_back(forNode);
        }
        currentNode = forNode;
    }

    void ShaderBuilder::EndFor() {
        assert(currentNode->type == TreeNode::FOR);
        currentNode = currentNode->parent;
    }

    void ShaderBuilder::Function(const std::string &name,
                                   const std::unordered_map<std::string, ShaderDataType> &arguments,
                                   ShaderDataType returnType) {
        // TODO: Redesign shader builder implementation to avoid leaking tree node pointers. (Valgrind reports this leaks so possibly a circular reference)
        std::shared_ptr<TreeNode> functionNode = std::make_shared<TreeNode>();
        functionNode->parent = nullptr;
        functionNode->type = TreeNode::ROOT;
        currentNode = functionNode;
        currentFunction.name = name;
        currentFunction.returnType = returnType;
        currentFunction.arguments = arguments;
        functionRoot = functionNode;
    }

    void ShaderBuilder::EndFunction() {
        ShaderFunction function = currentFunction;
        function.body = createNodes(*functionRoot);
        functions[function.name] = function;
        currentNode = rootNode;
    }

    std::string ShaderBuilder::getVariableName() {
        return "v" + std::to_string(variableCounter++);
    }

    std::vector<std::unique_ptr<ShaderNode> > ShaderBuilder::createNodes(TreeNode &node) {
        std::vector<std::unique_ptr<ShaderNode> > nodes;
        if (node.type == TreeNode::ROOT) {
            for (auto &child: node.defaultBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    nodes.push_back(childNode->copy());
                }
            }
        } else if (node.type == TreeNode::NODE) {
            nodes.push_back(node.node->copy());
        } else if (node.type == TreeNode::IF) {
            std::vector<std::unique_ptr<ShaderNode> > trueBranch;
            std::vector<std::unique_ptr<ShaderNode> > falseBranch;
            for (auto &child: node.defaultBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    trueBranch.push_back(childNode->copy());
                }
            }
            for (auto &child: node.falseBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    falseBranch.push_back(childNode->copy());
                }
            }
            nodes.push_back(ShaderNodeFactory::branch(node.condition->copy(), trueBranch, falseBranch));
        } else if (node.type == TreeNode::FOR) {
            std::vector<std::unique_ptr<ShaderNode> > body;
            for (auto &child: node.defaultBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    body.push_back(childNode->copy());
                }
            }

            auto initializer = ShaderNodeFactory::assign(node.loopVariable,
                                                       node.initializer);

            auto incrementor = ShaderNodeFactory::assign(node.loopVariable, node.incrementor);

            nodes.push_back(ShaderNodeFactory::loop(initializer,
                                                  node.condition,
                                                  incrementor, body));
        }
        return nodes;
    }

    void ShaderBuilder::addNode(const std::unique_ptr<ShaderNode> &node) {
        const std::shared_ptr<TreeNode> treeNode = std::make_shared<TreeNode>();
        treeNode->parent = currentNode;
        treeNode->type = TreeNode::NODE;
        treeNode->node = node->copy();

        if (currentNode->processingElse) {
            currentNode->falseBranch.push_back(treeNode);
        } else {
            currentNode->defaultBranch.push_back(treeNode);
        }
    }

    void ShaderBuilder::setup(ShaderStage::Type stage,
                                const ShaderAttributeLayout &inputLayout,
                                const ShaderAttributeLayout &outputLayout,
                                const std::unordered_map<std::string, ShaderDataType> &parameters,
                                const std::unordered_map<std::string, ShaderBuffer> &buffers,
                                const std::unordered_map<std::string, RenderGraphTexture> &textures,
                                const std::unordered_map<std::string, ShaderFunction> &functions) {
        rootNode = std::make_shared<TreeNode>();
        rootNode->parent = nullptr;
        rootNode->type = TreeNode::ROOT;
        currentNode = rootNode;

        functionRoot = nullptr;

        variableCounter = 0;

        currentFunction = {};

        this->stage = stage;
        this->inputLayout = inputLayout;
        this->outputLayout = outputLayout;
        this->parameters = parameters;
        this->buffers = buffers;
        this->textures = textures;
        this->functions = functions;
    }

    ShaderStage ShaderBuilder::build() {
        ShaderStage ret;
        ret.type = stage;
        ret.inputLayout = inputLayout;
        ret.outputLayout = outputLayout;
        ret.parameters = parameters;
        ret.buffers = buffers;
        ret.textures = textures;

        ret.mainFunction = createNodes(*rootNode);
        ret.functions = functions;

        return ret;
    }
}
