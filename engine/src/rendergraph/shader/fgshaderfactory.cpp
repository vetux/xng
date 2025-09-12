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

#include "xng/rendergraph/shader/fgshaderfactory.hpp"

#include "xng/rendergraph/shader/nodes.hpp"
#include "xng/rendergraph/shader/nodes/fgnodeattributeoutput.hpp"

namespace xng::FGShaderFactory {
    std::unique_ptr<FGShaderNode> createVariable(const std::string &name,
                                                 const FGShaderValue &type,
                                                 const std::unique_ptr<FGShaderNode> &value,
                                                 size_t count) {
        return std::make_unique<FGNodeVariableCreate>(name, type, count, value ? value->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> assign(const std::unique_ptr<FGShaderNode> &target,
                                         const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeAssign>(target->copy(), value->copy());
    }

    std::unique_ptr<FGShaderNode> variable(const std::string &name) {
        return std::make_unique<FGNodeVariable>(name);
    }

    std::unique_ptr<FGShaderNode> literal(const FGShaderLiteral &value) {
        return std::make_unique<FGNodeLiteral>(value);
    }

    std::unique_ptr<FGShaderNode> argument(const std::string &name) {
        return std::make_unique<FGNodeArgument>(name);
    }

    std::unique_ptr<FGShaderNode> attributeInput(uint32_t attributeIndex) {
        return std::make_unique<FGNodeAttributeInput>(attributeIndex);
    }

    std::unique_ptr<FGShaderNode> attributeOutput(uint32_t attributeIndex) {
        return std::make_unique<FGNodeAttributeOutput>(attributeIndex);
    }

    std::unique_ptr<FGShaderNode> parameter(std::string parameter_name) {
        return std::make_unique<FGNodeParameter>(parameter_name);
    }

    std::unique_ptr<FGShaderNode> vector(const FGShaderValue type,
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

    std::unique_ptr<FGShaderNode> array(FGShaderValue elementType,
                                        const std::vector<std::unique_ptr<FGShaderNode> > &elements) {
        std::vector<std::unique_ptr<FGShaderNode> > elementsCopy;
        elementsCopy.reserve(elements.size());
        for (auto &element: elements) {
            elementsCopy.push_back(element->copy());
        }
        return std::make_unique<FGNodeArray>(elementType, std::move(elementsCopy));
    }

    std::unique_ptr<FGShaderNode> textureSample(const std::string &textureName,
                                                const std::unique_ptr<FGShaderNode> &coordinate,
                                                const std::unique_ptr<FGShaderNode> &bias) {
        return std::make_unique<FGNodeTextureSample>(textureName, coordinate->copy(), bias ? bias->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> textureSize(const std::string &textureName) {
        return std::make_unique<FGNodeTextureSize>(textureName);
    }

    std::unique_ptr<FGShaderNode> bufferRead(const std::string &bufferName,
                                             const std::string &elementName,
                                             const std::unique_ptr<FGShaderNode> &index) {
        return std::make_unique<FGNodeBufferRead>(bufferName, elementName, index ? index->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> bufferWrite(const std::string &bufferName,
                                              const std::string &elementName,
                                              const std::unique_ptr<FGShaderNode> &index,
                                              const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBufferWrite>(bufferName,
                                                   elementName,
                                                   value->copy(),
                                                   index ? index->copy() : nullptr);
    }

    std::unique_ptr<FGShaderNode> bufferSize(const std::string &bufferName) {
        return std::make_unique<FGNodeBufferSize>(bufferName);
    }

    std::unique_ptr<FGShaderNode> add(const std::unique_ptr<FGShaderNode> &left,
                                      const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeAdd>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> subtract(const std::unique_ptr<FGShaderNode> &left,
                                           const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeSubtract>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> multiply(const std::unique_ptr<FGShaderNode> &left,
                                           const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeMultiply>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> divide(const std::unique_ptr<FGShaderNode> &left,
                                         const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeDivide>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> compareEqual(const std::unique_ptr<FGShaderNode> &left,
                                               const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> compareNotEqual(const std::unique_ptr<FGShaderNode> &left,
                                                  const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeNotEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> compareGreater(const std::unique_ptr<FGShaderNode> &left,
                                                 const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeGreater>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> compareLess(const std::unique_ptr<FGShaderNode> &left,
                                              const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeLess>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> compareGreaterEqual(const std::unique_ptr<FGShaderNode> &left,
                                                      const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeGreaterEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> compareLessEqual(const std::unique_ptr<FGShaderNode> &left,
                                                   const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeLessEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> logicalAnd(const std::unique_ptr<FGShaderNode> &left,
                                             const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeAnd>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> logicalOr(const std::unique_ptr<FGShaderNode> &left,
                                            const std::unique_ptr<FGShaderNode> &right) {
        return std::make_unique<FGNodeOr>(left->copy(), right->copy());
    }

    std::unique_ptr<FGShaderNode> call(const std::string &functionName,
                                       const std::vector<std::unique_ptr<FGShaderNode> > &arguments) {
        std::vector<std::unique_ptr<FGShaderNode> > argumentsCopy;
        argumentsCopy.reserve(arguments.size());
        for (auto &argument: arguments) {
            argumentsCopy.push_back(argument->copy());
        }
        return std::make_unique<FGNodeCall>(functionName, std::move(argumentsCopy));
    }

    std::unique_ptr<FGShaderNode> ret(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeReturn>(value->copy());
    }

    std::unique_ptr<FGShaderNode> abs(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ABS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> sin(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::SIN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> cos(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::COS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> tan(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::TAN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> asin(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ASIN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> acos(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ACOS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> atan(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ATAN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> pow(const std::unique_ptr<FGShaderNode> &base,
                                      const std::unique_ptr<FGShaderNode> &exponent) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::POW, base->copy(), exponent->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> exp(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::EXP, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> log(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::LOG, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> sqrt(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::SQRT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> inverseSqrt(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::INVERSESQRT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> floor(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::FLOOR, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> ceil(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::CEIL, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> round(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::ROUND, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> fract(const std::unique_ptr<FGShaderNode> &value) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::FRACT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> mod(const std::unique_ptr<FGShaderNode> &x,
                                      const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MOD, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> min(const std::unique_ptr<FGShaderNode> &x,
                                      const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MIN, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> max(const std::unique_ptr<FGShaderNode> &x,
                                      const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MAX, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> clamp(const std::unique_ptr<FGShaderNode> &x,
                                        const std::unique_ptr<FGShaderNode> &min,
                                        const std::unique_ptr<FGShaderNode> &max) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::CLAMP, x->copy(), min->copy(), max->copy());
    }

    std::unique_ptr<FGShaderNode> mix(const std::unique_ptr<FGShaderNode> &x,
                                      const std::unique_ptr<FGShaderNode> &y,
                                      const std::unique_ptr<FGShaderNode> &a) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::MIX, x->copy(), y->copy(), a->copy());
    }

    std::unique_ptr<FGShaderNode> step(const std::unique_ptr<FGShaderNode> &edge,
                                       const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::STEP, edge->copy(), x->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> smoothstep(const std::unique_ptr<FGShaderNode> &edge0,
                                             const std::unique_ptr<FGShaderNode> &edge1,
                                             const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::SMOOTHSTEP, edge0->copy(), edge1->copy(), x->copy());
    }

    std::unique_ptr<FGShaderNode> dot(const std::unique_ptr<FGShaderNode> &x,
                                      const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::DOT, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> cross(const std::unique_ptr<FGShaderNode> &x,
                                        const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::CROSS, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> normalize(const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::NORMALIZE, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> length(const std::unique_ptr<FGShaderNode> &x) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::LENGTH, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<FGShaderNode> distance(const std::unique_ptr<FGShaderNode> &x,
                                           const std::unique_ptr<FGShaderNode> &y) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::DISTANCE, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> reflect(const std::unique_ptr<FGShaderNode> &i,
                                          const std::unique_ptr<FGShaderNode> &n) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::REFLECT, i->copy(), n->copy(), nullptr);
    }

    std::unique_ptr<FGShaderNode> refract(const std::unique_ptr<FGShaderNode> &i,
                                          const std::unique_ptr<FGShaderNode> &n,
                                          const std::unique_ptr<FGShaderNode> &eta) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::REFRACT, i->copy(), n->copy(), eta->copy());
    }

    std::unique_ptr<FGShaderNode> faceforward(const std::unique_ptr<FGShaderNode> &n,
                                              const std::unique_ptr<FGShaderNode> &i,
                                              const std::unique_ptr<FGShaderNode> &nref) {
        return std::make_unique<FGNodeBuiltin>(FGNodeBuiltin::FACEFORWARD, n->copy(), i->copy(), nref->copy());
    }

    std::unique_ptr<FGShaderNode> subscriptArray(const std::unique_ptr<FGShaderNode> &array,
                                                 const std::unique_ptr<FGShaderNode> &index) {
        return std::make_unique<FGNodeSubscriptArray>(array->copy(), index->copy());
    }

    std::unique_ptr<FGShaderNode> subscriptVector(const std::unique_ptr<FGShaderNode> &value,
                                                  int index) {
        return std::make_unique<FGNodeSubscriptVector>(value->copy(), index);
    }

    std::unique_ptr<FGShaderNode> subscriptMatrix(const std::unique_ptr<FGShaderNode> &matrix,
                                                  const std::unique_ptr<FGShaderNode> &row,
                                                  const std::unique_ptr<FGShaderNode> &column) {
        return std::make_unique<FGNodeSubscriptMatrix>(matrix->copy(), row->copy(), column->copy());
    }

    std::unique_ptr<FGShaderNode> branch(const std::unique_ptr<FGShaderNode> &condition,
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

    std::unique_ptr<FGShaderNode> loop(const std::unique_ptr<FGShaderNode> &initializer,
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
}
