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

    std::unique_ptr<FGShaderNode> FGShaderBuilder::vector(const FGShaderValue type,
                                                          const std::unique_ptr<FGShaderNode> &x,
                                                          const std::unique_ptr<FGShaderNode> &y,
                                                          const std::unique_ptr<FGShaderNode> &z,
                                                          const std::unique_ptr<FGShaderNode> &w) {
        return std::make_unique<FGNodeVector>(type,
                                              x->copy(),
                                              y->copy(),
                                              z ? z->copy() : nullptr,
                                              w ? w->copy() : nullptr);
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

    std::unique_ptr<FGShaderNode> FGShaderBuilder::ret(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeReturn>(value->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::abs(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ABS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::sin(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::SIN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::cos(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::COS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::tan(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::TAN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::asin(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ASIN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::acos(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ACOS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::atan(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ATAN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::pow(const std::unique_ptr<FGShaderNode> &base,
                                                       const std::unique_ptr<FGShaderNode> &exponent) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::POW, base->copy(), exponent->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::exp(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::EXP, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::log(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::LOG, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::sqrt(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::SQRT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::inverseSqrt(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::INVERSESQRT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::floor(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::FLOOR, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::ceil(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::CEIL, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::round(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ROUND, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::fract(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::FRACT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::mod(const std::unique_ptr<FGShaderNode> &x,
                                                       const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MOD, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::min(const std::unique_ptr<FGShaderNode> &x,
                                                       const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MIN, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::max(const std::unique_ptr<FGShaderNode> &x,
                                                       std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MAX, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::clamp(const std::unique_ptr<FGShaderNode> &x,
                                                         const std::unique_ptr<FGShaderNode> &min,
                                                         const std::unique_ptr<FGShaderNode> &max) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::CLAMP, x->copy(), min->copy(), max->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::mix(const std::unique_ptr<FGShaderNode> &x,
                                                       const std::unique_ptr<FGShaderNode> &y,
                                                       const std::unique_ptr<FGShaderNode> &a) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MIX, x->copy(), y->copy(), a->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::step(const std::unique_ptr<FGShaderNode> &edge,
                                                        const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::STEP, edge->copy(), x->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::smoothstep(const std::unique_ptr<FGShaderNode> &edge0,
                                                              const std::unique_ptr<FGShaderNode> &edge1,
                                                              const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::SMOOTHSTEP, edge0->copy(), edge1->copy(), x->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::dot(const std::unique_ptr<FGShaderNode> &x,
                                                       const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::DOT, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::cross(const std::unique_ptr<FGShaderNode> &x,
                                                         const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::CROSS, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::normalize(const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::NORMALIZE, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::length(const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::LENGTH, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::distance(const std::unique_ptr<FGShaderNode> &x,
                                                            const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::DISTANCE, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::reflect(const std::unique_ptr<FGShaderNode> &i,
                                                           const std::unique_ptr<FGShaderNode> &n) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::REFLECT, i->copy(), n->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::refract(const std::unique_ptr<FGShaderNode> &i,
                                                           const std::unique_ptr<FGShaderNode> &n,
                                                           const std::unique_ptr<FGShaderNode> &eta) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::REFRACT, i->copy(), n->copy(), eta->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::faceforward(const std::unique_ptr<FGShaderNode> &n,
                                                               const std::unique_ptr<FGShaderNode> &i,
                                                               const std::unique_ptr<FGShaderNode> &nref) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::FACEFORWARD, n->copy(), i->copy(), nref->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::subscriptArray(const std::unique_ptr<FGShaderNode> &array,
                                                                  const std::unique_ptr<FGShaderNode> &index) {
        return std::make_unique<FGNodeSubscriptArray>(array->copy(), index->copy());
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::subscriptVector(const std::unique_ptr<FGShaderNode> &value,
                                                                   int index) {
        return std::make_unique<FGNodeSubscriptVector>(value->copy(), index);
    }

    std::unique_ptr<FGShaderNode> FGShaderBuilder::subscriptMatrix(const std::unique_ptr<FGShaderNode> &matrix,
                                                                   const std::unique_ptr<FGShaderNode> &row,
                                                                   const std::unique_ptr<FGShaderNode> &column) {
        return std::make_unique<FGNodeSubscriptMatrix>(matrix->copy(), row->copy(), column->copy());
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

    std::unique_ptr<FGShaderNode> FGShaderBuilder::loop(const std::unique_ptr<FGShaderNode> &initializer,
                                                        const std::unique_ptr<FGShaderNode> &predicate,
                                                        const std::unique_ptr<FGShaderNode> &iterator,
                                                        const std::vector<std::unique_ptr<FGShaderNode> > &body) {
        std::vector<std::unique_ptr<FGShaderNode> > bodyCopy;
        bodyCopy.reserve(body.size());
        for (auto &node: body) {
            bodyCopy.push_back(node->copy());
        }
        return std::make_unique<FGNodeLoop>(initializer->copy(),
                                            predicate->copy(),
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
