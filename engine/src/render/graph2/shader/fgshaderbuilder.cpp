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

#include "xng/render/graph2/shader/nodes.hpp"
#include "xng/render/graph2/shader/nodes/fgnodeargument.hpp"
#include "xng/render/graph2/shader/nodes/fgnodecall.hpp"
#include "xng/render/graph2/shader/nodes/fgnodevariablecreate.hpp"
#include "xng/render/graph2/shader/nodes/fgnodevariableread.hpp"
#include "xng/render/graph2/shader/nodes/fgnodevariablewrite.hpp"

namespace xng {
    std::unique_ptr<FGShaderNode> FGShaderBuilder::createVariable(const std::string &name,
                                                                  const FGShaderValue &type,
                                                                  const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeVariableCreate>(name, type, value ? value->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::assignVariable(const std::string &variableName,
                                                                  const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeVariableWrite>(variableName, value->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::variable(const std::string &name) {
        return std::make_unique<FGNodeVariableRead>(name);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::literal(const FGShaderLiteral &value) {
        return std::make_unique<FGNodeLiteral>(value);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::argument(const std::string &name) {
        return std::make_unique<FGNodeArgument>(name);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::vector(const std::unique_ptr<FGShaderNode> &x,
                                                          const std::unique_ptr<FGShaderNode> &y,
                                                          const std::unique_ptr<FGShaderNode> &z,
                                                          const std::unique_ptr<FGShaderNode> &w) {
        return std::make_unique<FGNodeVector>(x->copy(), y->copy(), z->copy(), w->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::attributeRead(int32_t attributeIndex) {
        return std::make_unique<FGNodeAttributeRead>(attributeIndex);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::attributeWrite(int32_t attributeIndex,
                                                                  const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeAttributeWrite>(attributeIndex, value->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::parameterRead(std::string parameter_name) {
        return std::make_unique<FGNodeParameterRead>(parameter_name);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::textureSample(const std::string &textureName,
                                                                 const std::unique_ptr<FGShaderNode> &coordinate,
                                                                 const std::unique_ptr<FGShaderNode> &bias) {
        return std::make_unique<FGNodeTextureSample>(textureName, coordinate->copy(), bias->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::textureSize(const std::string &textureName) {
        return std::make_unique<FGNodeTextureSize>(textureName);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::bufferRead(const std::string &bufferName,
                                                              const std::string &elementName,
                                                              const std::unique_ptr<FGShaderNode> &index) {
        return std::make_unique<FGNodeBufferRead>(bufferName, elementName, index ? index->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::bufferWrite(const std::string &bufferName,
                                                               const std::string &elementName,
                                                               const std::unique_ptr<FGShaderNode> &value,
                                                               const std::unique_ptr<FGShaderNode> &index) {
        return std::make_unique<FGNodeBufferWrite>(bufferName,
                                                   elementName,
                                                   value->copy(),
                                                   index ? index->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::bufferSize(const std::string &bufferName) {
        return std::make_unique<FGNodeBufferSize>(bufferName);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::add(const std::unique_ptr<FGShaderNode> &left,
                                                       const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeAdd>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::subtract(const std::unique_ptr<FGShaderNode> &left,
                                                            const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeSubtract>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::multiply(const std::unique_ptr<FGShaderNode> &left,
                                                            const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeMultiply>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::divide(const std::unique_ptr<FGShaderNode> &left,
                                                          const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeDivide>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::compareEqual(const std::unique_ptr<FGShaderNode> &left,
                                                                const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::compareNotEqual(const std::unique_ptr<FGShaderNode> &left,
                                                                   const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeNotEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::compareGreater(const std::unique_ptr<FGShaderNode> &left,
                                                                  const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeGreater>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::compareLess(const std::unique_ptr<FGShaderNode> &left,
                                                               const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeLess>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::compareGreaterEqual(const std::unique_ptr<FGShaderNode> &left,
                                                                       const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeGreaterEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::compareLessEqual(const std::unique_ptr<FGShaderNode> &left,
                                                                    const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeLessEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::logicalAnd(const std::unique_ptr<FGShaderNode> &left,
                                                              const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeAnd>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::logicalOr(const std::unique_ptr<FGShaderNode> &left,
                                                             const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeOr>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::call(const std::string &functionName,
                                                        const std::vector<std::unique_ptr<FGShaderNode> > &arguments) {
        std::vector<std::unique_ptr<FGShaderNode> > argumentsCopy;
        argumentsCopy.reserve(arguments.size());
        for (auto &argument: arguments) {
            argumentsCopy.push_back(argument->copy());
        }
        return std::make_unique<FGNodeCall>(functionName, std::move(argumentsCopy));
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::normalize(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeNormalize>(value->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::subscript(const std::unique_ptr<FGShaderNode> &value,
                                                             const std::unique_ptr<FGShaderNode> &row,
                                                             const std::unique_ptr<FGShaderNode> &column) {
        return std::make_unique<FGNodeSubscript>(value->copy(), row->copy(), column ? column->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::branch(const std::unique_ptr<FGShaderNode> &condition,
                                                          const std::vector<std::unique_ptr<FGShaderNode> > &trueBranch,
                                                          const std::vector<std::unique_ptr<FGShaderNode> > &
                                                          falseBranch) {
        std::vector<std::unique_ptr<FGShaderNode> > trueBranchCopy;
        trueBranchCopy.reserve(trueBranch.size());
        for (auto &node: trueBranch) {
            trueBranchCopy.push_back(node->copy());
        }
        std::vector<std::unique_ptr<FGShaderNode> > falseBranchCopy;
        falseBranchCopy.reserve(falseBranch.size());
        for (auto &node: falseBranch) {
            falseBranchCopy.push_back(node->copy());
        }
        return std::make_unique<FGNodeBranch>(condition->copy(),
                                              std::move(trueBranchCopy),
                                              std::move(falseBranchCopy));
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::loop(const std::unique_ptr<FGShaderNode> &predicate,
                                                        const std::unique_ptr<FGShaderNode> &initializer,
                                                        const std::unique_ptr<FGShaderNode> &iterator,
                                                        const std::vector<std::unique_ptr<FGShaderNode> > &body) {
        std::vector<std::unique_ptr<FGShaderNode> > bodyCopy;
        bodyCopy.reserve(body.size());
        for (auto &node: body) {
            bodyCopy.push_back(node->copy());
        }
        return std::make_unique<FGNodeLoop>(predicate->copy(),
                                            initializer->copy(),
                                            iterator->copy(),
                                            std::move(bodyCopy));
    }

    void FGShaderBuilder::defineFunction(const std::string &name,
                                         const std::vector<std::unique_ptr<FGShaderNode> > &body,
                                         const std::unordered_map<std::string, FGShaderValue> &arguments,
                                         const FGShaderValue returnType) {
        std::vector<std::unique_ptr<FGShaderNode> > bodyCopy;
        bodyCopy.reserve(body.size());
        for (auto &node: body) {
            bodyCopy.push_back(node->copy());
        }
        functions.emplace(name, FGShaderFunction(name, arguments, std::move(bodyCopy), returnType));
    }

    FGShaderSource FGShaderBuilder::build(FGShaderSource::ShaderStage stage, const FGAttributeLayout &inputLayout,
                                          const FGAttributeLayout &outputLayout,
                                          const std::unordered_map<std::string, FGShaderValue> &parameters,
                                          const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                                          const std::unordered_map<std::string, FGTexture> &textures,
                                          const std::vector<std::unique_ptr<FGShaderNode> > &mainFunction) {
        std::vector<std::unique_ptr<FGShaderNode> > mainFunctionCopy;
        mainFunctionCopy.reserve(mainFunction.size());
        for (const auto &node: mainFunction) {
            mainFunctionCopy.push_back(node->copy());
        }
        return {
            stage,
            inputLayout,
            outputLayout,
            parameters,
            buffers,
            textures,
            std::move(mainFunctionCopy),
            std::move(functions)
        };
    }
}
