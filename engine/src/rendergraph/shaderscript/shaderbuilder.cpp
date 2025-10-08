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
#include "xng/rendergraph/shaderscript/shaderobject.hpp"

namespace xng::ShaderScript {
    thread_local ShaderBuilder fgShaderBuilder;

    ShaderBuilder &ShaderBuilder::instance() {
        return fgShaderBuilder;
    }

    void ShaderBuilder::If(const ShaderObject &condition) {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::If called without a setup() call");
        }
        std::shared_ptr<TreeNode> ifNode = std::make_shared<TreeNode>();
        ifNode->parent = currentNode;
        ifNode->type = TreeNode::IF;
        ifNode->condition = condition.operand;
        if (currentNode->processingElse) {
            currentNode->falseBranch.push_back(ifNode);
        } else {
            currentNode->defaultBranch.push_back(ifNode);
        }
        currentNode = ifNode.get();
    }

    void ShaderBuilder::Else() {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::Else called without a setup() call");
        }
        if (currentNode->type != TreeNode::IF) {
            throw std::runtime_error("ShaderBuilder::Else called without a If() call");
        }
        currentNode->processingElse = true;
    }

    void ShaderBuilder::EndIf() {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::EndIf called without a setup() call");
        }
        if (currentNode->type != TreeNode::IF) {
            throw std::runtime_error("ShaderBuilder::EndIf called without a If() call");
        }
        currentNode = currentNode->parent;
    }

    void ShaderBuilder::For(const ShaderObject &loopVariable,
                            const ShaderObject &loopStart,
                            const ShaderObject &loopEnd,
                            const ShaderObject &incrementor) {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::For called without a setup() call");
        }
        std::shared_ptr<TreeNode> forNode = std::make_shared<TreeNode>();
        forNode->parent = currentNode;
        forNode->type = TreeNode::FOR;
        forNode->loopVariable = loopVariable.operand;
        forNode->loopEnd = loopEnd.operand;
        forNode->initializer = loopStart.operand;
        forNode->incrementor = incrementor.operand;
        if (currentNode->processingElse) {
            currentNode->falseBranch.push_back(forNode);
        } else {
            currentNode->defaultBranch.push_back(forNode);
        }
        currentNode = forNode.get();
    }

    void ShaderBuilder::EndFor() {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::EndFor called without a setup() call");
        }
        if (currentNode->type != TreeNode::FOR) {
            throw std::runtime_error("ShaderBuilder::EndFor called without a For() call");
        }
        currentNode = currentNode->parent;
    }

    void ShaderBuilder::Function(const std::string &name,
                                 const std::vector<ShaderFunction::Argument> &arguments,
                                 ShaderFunction::ReturnType returnType) {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::Function called without a setup() call");
        }
        if (functionRoot != nullptr) {
            throw std::runtime_error(
                "ShaderBuilder::Function called while a function is in progress (Nested function definition?)");
        }
        std::shared_ptr<TreeNode> functionNode = std::make_shared<TreeNode>();
        functionNode->parent = nullptr;
        functionNode->type = TreeNode::ROOT;
        currentNode = functionNode.get();
        currentFunction.name = name;
        currentFunction.returnType = std::move(returnType);
        currentFunction.arguments = arguments;
        functionRoot = functionNode;
    }

    void ShaderBuilder::EndFunction() {
        if (functionRoot == nullptr) {
            throw std::runtime_error("ShaderBuilder::EndFunction called without a Function() call");
        }
        ShaderFunction function = currentFunction;
        function.body = createNodes(*functionRoot);
        functions.emplace_back(function);
        currentNode = rootNode.get();
        currentFunction = {};
        functionRoot = nullptr;
    }

    std::string ShaderBuilder::getVariableName() {
        return "v" + std::to_string(variableCounter++);
    }

    std::vector<ShaderInstruction> ShaderBuilder::createNodes(TreeNode &node) {
        std::vector<ShaderInstruction> nodes;
        if (node.type == TreeNode::ROOT) {
            for (auto &child: node.defaultBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    nodes.push_back(childNode);
                }
            }
        } else if (node.type == TreeNode::NODE) {
            nodes.push_back(node.node);
        } else if (node.type == TreeNode::IF) {
            std::vector<ShaderInstruction> trueBranch;
            std::vector<ShaderInstruction> falseBranch;
            for (auto &child: node.defaultBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    trueBranch.push_back(childNode);
                }
            }
            for (auto &child: node.falseBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    falseBranch.push_back(childNode);
                }
            }
            nodes.push_back(ShaderInstructionFactory::branch(
                node.condition,
                trueBranch,
                falseBranch));
        } else if (node.type == TreeNode::FOR) {
            std::vector<ShaderInstruction> body;
            for (auto &child: node.defaultBranch) {
                auto childNodes = createNodes(*child);
                for (auto &childNode: childNodes) {
                    body.push_back(childNode);
                }
            }

            if (node.loopVariable.type != ShaderOperand::Variable) {
                throw std::runtime_error("Invalid For loop variable");
            }

            auto initializer = ShaderInstructionFactory::assign(node.loopVariable, node.initializer);
            auto incrementor = ShaderInstructionFactory::assign(node.loopVariable,
                                                                ShaderOperand(
                                                                    ShaderInstructionFactory::add(
                                                                        node.loopVariable, node.incrementor)));

            auto condition = ShaderInstructionFactory::compareLessEqual(node.loopVariable, node.loopEnd);

            nodes.push_back(ShaderInstruction(initializer));
            nodes.push_back(ShaderInstruction(ShaderInstructionFactory::loop(ShaderOperand(initializer),
                                                                         ShaderOperand(condition),
                                                                         ShaderOperand(incrementor),
                                                                         body)));
        }
        return nodes;
    }

    void ShaderBuilder::addInstruction(const ShaderInstruction &node) {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::addNode called without a setup() call");
        }

        const std::shared_ptr<TreeNode> treeNode = std::make_shared<TreeNode>();
        treeNode->parent = currentNode;
        treeNode->type = TreeNode::NODE;
        treeNode->node = node;

        if (currentNode->processingElse) {
            currentNode->falseBranch.push_back(treeNode);
        } else {
            currentNode->defaultBranch.push_back(treeNode);
        }
    }

    void ShaderBuilder::setup(Shader::Stage stage) {
        if (currentNode != nullptr) {
            throw std::runtime_error("ShaderBuilder::setup called while a build is in progress (Nested setup() call?)");
        }
        rootNode = std::make_shared<TreeNode>();
        rootNode->parent = nullptr;
        rootNode->type = TreeNode::ROOT;
        currentNode = rootNode.get();

        this->stage = stage;
    }

    Shader ShaderBuilder::build() {
        if (currentNode == nullptr) {
            throw std::runtime_error("ShaderBuilder::build called without a setup() call");
        }

        if (currentNode->type != TreeNode::ROOT) {
            throw std::runtime_error("Attempting to call build with unfinished shader. (Missing EndIf, EndFor ?)");
        }

        if (functionRoot != nullptr) {
            throw std::runtime_error("Attempting to call build with unfinished shader. (Missing EndFunction ?)");
        }

        Shader ret;
        ret.stage = stage;
        ret.inputLayout = inputLayout;
        ret.outputLayout = outputLayout;
        ret.geometryInput = geometryInput;
        ret.geometryOutput = geometryOutput;
        ret.geometryMaxVertices = geometryMaxVertices;
        ret.parameters = parameters;
        ret.buffers = buffers;
        ret.textureArrays = textureArrays;
        ret.typeDefinitions = typeDefinitions;

        ret.mainFunction = createNodes(*rootNode);
        ret.functions = functions;

        inputLayout = {};
        outputLayout = {};
        geometryInput = {};
        geometryOutput = {};
        geometryMaxVertices = 0;
        parameters = {};
        buffers = {};
        textureArrays = {};
        typeDefinitions = {};

        currentNode = nullptr;
        rootNode = {};

        functionRoot = {};
        functions = {};

        variableCounter = 0;

        currentFunction = {};

        return ret;
    }
}
