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

#include "xng/render/graph2/shader/fgshaderbuilder.hpp"

#include "xng/render/graph2/shader/fgshadernodewrapper.hpp"

namespace xng {
    thread_local FGShaderBuilder fgShaderBuilder;

    FGShaderBuilder &FGShaderBuilder::instance() {
        return fgShaderBuilder;
    }

    void FGShaderBuilder::If(const FGShaderNodeWrapper &condition) {
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

    void FGShaderBuilder::Else() {
        assert(currentNode->type == TreeNode::IF);
        currentNode->processingElse = true;
    }

    void FGShaderBuilder::EndIf() {
        assert(currentNode->type == TreeNode::IF);
        currentNode = currentNode->parent;
    }

    void FGShaderBuilder::For(const FGShaderNodeWrapper &loopVariable,
                              const FGShaderNodeWrapper &initializer,
                              const FGShaderNodeWrapper &condition,
                              const FGShaderNodeWrapper &incrementor) {
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

    void FGShaderBuilder::EndFor() {
        assert(currentNode->type == TreeNode::FOR);
        currentNode = currentNode->parent;
    }

    void FGShaderBuilder::Function(const std::string &name,
                                   const std::unordered_map<std::string, FGShaderValue> &arguments,
                                   FGShaderValue returnType) {
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

    void FGShaderBuilder::EndFunction() {
        FGShaderFunction function = currentFunction;
        function.body = createNodes(*functionRoot);
        functions[function.name] = function;
        currentNode = rootNode;
    }

    std::string FGShaderBuilder::getVariableName() {
        return "v" + std::to_string(variableCounter++);
    }

    std::vector<std::unique_ptr<FGShaderNode> > FGShaderBuilder::createNodes(TreeNode &node) {
        std::vector<std::unique_ptr<FGShaderNode> > nodes;
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
            std::vector<std::unique_ptr<FGShaderNode> > trueBranch;
            std::vector<std::unique_ptr<FGShaderNode> > falseBranch;
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
            nodes.push_back(FGShaderFactory::branch(node.condition->copy(), trueBranch, falseBranch));
        } else if (node.type == TreeNode::FOR) {
            std::vector<std::unique_ptr<FGShaderNode> > body;
            for (auto &child: node.defaultBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    body.push_back(childNode->copy());
                }
            }

            auto initializer = FGShaderFactory::assign(node.loopVariable,
                                                       node.initializer);

            auto incrementor = FGShaderFactory::assign(node.loopVariable, node.incrementor);

            nodes.push_back(FGShaderFactory::loop(initializer,
                                                  node.condition,
                                                  incrementor, body));
        }
        return nodes;
    }

    void FGShaderBuilder::addNode(const std::unique_ptr<FGShaderNode> &node) {
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

    void FGShaderBuilder::setup(FGShaderSource::ShaderStage stage,
                                const FGAttributeLayout &inputLayout,
                                const FGAttributeLayout &outputLayout,
                                const std::unordered_map<std::string, FGShaderValue> &parameters,
                                const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                                const std::unordered_map<std::string, FGTexture> &textures,
                                const std::unordered_map<std::string, FGShaderFunction> &functions) {
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

    FGShaderSource FGShaderBuilder::build() {
        FGShaderSource ret;
        ret.stage = stage;
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
