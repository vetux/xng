/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#include "xng/rendergraph/shader/shadernodefactory.hpp"

#include "xng/rendergraph/shader/nodes.hpp"
#include "xng/rendergraph/shader/nodes/nodeattributeoutput.hpp"
#include "xng/rendergraph/shader/nodes/nodetexture.hpp"
#include "xng/rendergraph/shader/nodes/nodetexturefetch.hpp"
#include "xng/rendergraph/shader/nodes/nodevertexposition.hpp"

namespace xng::ShaderNodeFactory {
    std::unique_ptr<ShaderNode> createVariable(const std::string &name,
                                               const std::variant<ShaderDataType, ShaderStructName> &type,
                                               const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeVariableCreate>(name, type, value ? value->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> assign(const std::unique_ptr<ShaderNode> &target,
                                       const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeAssign>(target->copy(), value->copy());
    }

    std::unique_ptr<ShaderNode> variable(const std::string &name) {
        return std::make_unique<NodeVariable>(name);
    }

    std::unique_ptr<ShaderNode> literal(const ShaderLiteral &value) {
        return std::make_unique<NodeLiteral>(value);
    }

    std::unique_ptr<ShaderNode> argument(const std::string &name) {
        return std::make_unique<NodeArgument>(name);
    }

    std::unique_ptr<ShaderNode> attributeInput(const std::string &attributeName) {
        return std::make_unique<NodeAttributeInput>(attributeName);
    }

    std::unique_ptr<ShaderNode> attributeOutput(const std::string &attributeName) {
        return std::make_unique<NodeAttributeOutput>(attributeName);
    }

    std::unique_ptr<ShaderNode> parameter(const std::string &parameter_name) {
        return std::make_unique<NodeParameter>(parameter_name);
    }

    std::unique_ptr<ShaderNode> vertexPosition(const std::unique_ptr<ShaderNode> &position) {
        return std::make_unique<NodeVertexPosition>(position->copy());
    }

    std::unique_ptr<ShaderNode> fragmentDepth(const std::unique_ptr<ShaderNode> &depth) {
        return std::make_unique<NodeFragmentDepth>(depth->copy());
    }

    std::unique_ptr<ShaderNode> emitVertex() {
        return std::make_unique<NodeEmitVertex>();
    }

    std::unique_ptr<ShaderNode> endPrimitive() {
        return std::make_unique<NodeEndPrimitive>();
    }

    std::unique_ptr<ShaderNode> layer() {
        return std::make_unique<NodeLayer>();
    }

    std::unique_ptr<ShaderNode> vector(const ShaderDataType type,
                                       const std::unique_ptr<ShaderNode> &x,
                                       const std::unique_ptr<ShaderNode> &y,
                                       const std::unique_ptr<ShaderNode> &z,
                                       const std::unique_ptr<ShaderNode> &w) {
        return std::make_unique<NodeVector>(type,
                                            x->copy(),
                                            y->copy(),
                                            z ? z->copy() : nullptr,
                                            w ? w->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> matrix(ShaderDataType type,
                                       const std::unique_ptr<ShaderNode> &x,
                                       const std::unique_ptr<ShaderNode> &y,
                                       const std::unique_ptr<ShaderNode> &z,
                                       const std::unique_ptr<ShaderNode> &w) {
        return std::make_unique<NodeMatrix>(type,
                                            x->copy(),
                                            y ? y->copy() : nullptr,
                                            z ? z->copy() : nullptr,
                                            w ? w->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> array(ShaderDataType elementType,
                                      const std::vector<std::unique_ptr<ShaderNode> > &elements) {
        std::vector<std::unique_ptr<ShaderNode> > elementsCopy;
        elementsCopy.reserve(elements.size());
        for (auto &element: elements) {
            elementsCopy.push_back(element->copy());
        }
        return std::make_unique<NodeArray>(elementType, std::move(elementsCopy));
    }

    std::unique_ptr<ShaderNode> texture(std::string textureName, const std::unique_ptr<ShaderNode> &textureIndex) {
        return std::make_unique<NodeTexture>(std::move(textureName), textureIndex ? textureIndex->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> textureSample(const std::unique_ptr<ShaderNode> &texture,
                                              const std::unique_ptr<ShaderNode> &coordinate,
                                              const std::unique_ptr<ShaderNode> &lod) {
        return std::make_unique<NodeTextureSample>(texture->copy(), coordinate->copy(), lod ? lod->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> textureSize(const std::unique_ptr<ShaderNode> &texture,
                                            const std::unique_ptr<ShaderNode> &lod) {
        return std::make_unique<NodeTextureSize>(texture->copy(), lod ? lod->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> textureFetch(const std::unique_ptr<ShaderNode> &texture,
                                             const std::unique_ptr<ShaderNode> &coordinate,
                                             const std::unique_ptr<ShaderNode> &index) {
        return std::make_unique<NodeTextureFetch>(texture->copy(),
                                                  coordinate->copy(),
                                                  index->copy());
    }

    std::unique_ptr<ShaderNode> buffer(const std::string &bufferName,
                                       const std::unique_ptr<ShaderNode> &index) {
        return std::make_unique<NodeBuffer>(bufferName, index ? index->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> bufferSize(const std::string &bufferName) {
        return std::make_unique<NodeBufferSize>(bufferName);
    }

    std::unique_ptr<ShaderNode> objectElement(const std::unique_ptr<ShaderNode> &object,
                                              const std::string &elementName) {
        return std::make_unique<NodeObjectElement>(object->copy(), elementName);
    }

    std::unique_ptr<ShaderNode> add(const std::unique_ptr<ShaderNode> &left,
                                    const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeAdd>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> subtract(const std::unique_ptr<ShaderNode> &left,
                                         const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeSubtract>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> multiply(const std::unique_ptr<ShaderNode> &left,
                                         const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeMultiply>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> divide(const std::unique_ptr<ShaderNode> &left,
                                       const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeDivide>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> compareEqual(const std::unique_ptr<ShaderNode> &left,
                                             const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> compareNotEqual(const std::unique_ptr<ShaderNode> &left,
                                                const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeNotEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> compareGreater(const std::unique_ptr<ShaderNode> &left,
                                               const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeGreater>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> compareLess(const std::unique_ptr<ShaderNode> &left,
                                            const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeLess>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> compareGreaterEqual(const std::unique_ptr<ShaderNode> &left,
                                                    const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeGreaterEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> compareLessEqual(const std::unique_ptr<ShaderNode> &left,
                                                 const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeLessEqual>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> logicalAnd(const std::unique_ptr<ShaderNode> &left,
                                           const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeAnd>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> logicalOr(const std::unique_ptr<ShaderNode> &left,
                                          const std::unique_ptr<ShaderNode> &right) {
        return std::make_unique<NodeOr>(left->copy(), right->copy());
    }

    std::unique_ptr<ShaderNode> call(const std::string &functionName,
                                     const std::vector<std::unique_ptr<ShaderNode> > &arguments) {
        std::vector<std::unique_ptr<ShaderNode> > argumentsCopy;
        argumentsCopy.reserve(arguments.size());
        for (auto &argument: arguments) {
            argumentsCopy.push_back(argument->copy());
        }
        return std::make_unique<NodeCall>(functionName, std::move(argumentsCopy));
    }

    std::unique_ptr<ShaderNode> ret(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeReturn>(value->copy());
    }


    std::unique_ptr<ShaderNode> ret() {
        return std::make_unique<NodeReturn>(nullptr);
    }

    std::unique_ptr<ShaderNode> abs(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::ABS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> sin(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::SIN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> cos(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::COS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> tan(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::TAN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> asin(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::ASIN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> acos(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::ACOS, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> atan(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::ATAN, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> pow(const std::unique_ptr<ShaderNode> &base,
                                    const std::unique_ptr<ShaderNode> &exponent) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::POW, base->copy(), exponent->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> exp(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::EXP, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> log(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::LOG, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> sqrt(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::SQRT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> inverseSqrt(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::INVERSESQRT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> floor(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::FLOOR, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> ceil(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::CEIL, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> round(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::ROUND, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> fract(const std::unique_ptr<ShaderNode> &value) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::FRACT, value->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> mod(const std::unique_ptr<ShaderNode> &x,
                                    const std::unique_ptr<ShaderNode> &y) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::MOD, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> min(const std::unique_ptr<ShaderNode> &x,
                                    const std::unique_ptr<ShaderNode> &y) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::MIN, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> max(const std::unique_ptr<ShaderNode> &x,
                                    const std::unique_ptr<ShaderNode> &y) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::MAX, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> clamp(const std::unique_ptr<ShaderNode> &x,
                                      const std::unique_ptr<ShaderNode> &min,
                                      const std::unique_ptr<ShaderNode> &max) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::CLAMP, x->copy(), min->copy(), max->copy());
    }

    std::unique_ptr<ShaderNode> mix(const std::unique_ptr<ShaderNode> &x,
                                    const std::unique_ptr<ShaderNode> &y,
                                    const std::unique_ptr<ShaderNode> &a) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::MIX, x->copy(), y->copy(), a->copy());
    }

    std::unique_ptr<ShaderNode> step(const std::unique_ptr<ShaderNode> &edge,
                                     const std::unique_ptr<ShaderNode> &x) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::STEP, edge->copy(), x->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> smoothstep(const std::unique_ptr<ShaderNode> &edge0,
                                           const std::unique_ptr<ShaderNode> &edge1,
                                           const std::unique_ptr<ShaderNode> &x) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::SMOOTHSTEP, edge0->copy(), edge1->copy(), x->copy());
    }

    std::unique_ptr<ShaderNode> dot(const std::unique_ptr<ShaderNode> &x,
                                    const std::unique_ptr<ShaderNode> &y) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::DOT, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> cross(const std::unique_ptr<ShaderNode> &x,
                                      const std::unique_ptr<ShaderNode> &y) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::CROSS, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> normalize(const std::unique_ptr<ShaderNode> &x) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::NORMALIZE, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> length(const std::unique_ptr<ShaderNode> &x) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::LENGTH, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> distance(const std::unique_ptr<ShaderNode> &x,
                                         const std::unique_ptr<ShaderNode> &y) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::DISTANCE, x->copy(), y->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> reflect(const std::unique_ptr<ShaderNode> &i,
                                        const std::unique_ptr<ShaderNode> &n) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::REFLECT, i->copy(), n->copy(), nullptr);
    }

    std::unique_ptr<ShaderNode> refract(const std::unique_ptr<ShaderNode> &i,
                                        const std::unique_ptr<ShaderNode> &n,
                                        const std::unique_ptr<ShaderNode> &eta) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::REFRACT, i->copy(), n->copy(), eta->copy());
    }

    std::unique_ptr<ShaderNode> faceforward(const std::unique_ptr<ShaderNode> &n,
                                            const std::unique_ptr<ShaderNode> &i,
                                            const std::unique_ptr<ShaderNode> &nref) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::FACEFORWARD, n->copy(), i->copy(), nref->copy());
    }

    std::unique_ptr<ShaderNode> transpose(const std::unique_ptr<ShaderNode> &x) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::TRANSPOSE, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> inverse(const std::unique_ptr<ShaderNode> &x) {
        return std::make_unique<NodeBuiltin>(NodeBuiltin::INVERSE, x->copy(), nullptr, nullptr);
    }

    std::unique_ptr<ShaderNode> arraySubscript(const std::unique_ptr<ShaderNode> &array,
                                               const std::unique_ptr<ShaderNode> &index) {
        return std::make_unique<NodeArraySubscript>(array->copy(), index->copy());
    }

    std::unique_ptr<ShaderNode> vectorSwizzle(const std::unique_ptr<ShaderNode> &value,
                                              const std::vector<NodeVectorSwizzle::ComponentIndex> &indices) {
        return std::make_unique<NodeVectorSwizzle>(value->copy(), indices);
    }

    std::unique_ptr<ShaderNode> matrixSubscript(const std::unique_ptr<ShaderNode> &matrix,
                                                const std::unique_ptr<ShaderNode> &column,
                                                const std::unique_ptr<ShaderNode> &row) {
        return std::make_unique<NodeMatrixSubscript>(matrix->copy(), column->copy(), row ? row->copy() : nullptr);
    }

    std::unique_ptr<ShaderNode> branch(const std::unique_ptr<ShaderNode> &condition,
                                       const std::vector<std::unique_ptr<ShaderNode> > &trueBranch,
                                       const std::vector<std::unique_ptr<ShaderNode> > &
                                       falseBranch) {
        std::vector<std::unique_ptr<ShaderNode> > trueBranchCopy;
        trueBranchCopy.reserve(trueBranch.size());
        for (auto &node: trueBranch) {
            trueBranchCopy.push_back(node->copy());
        }
        std::vector<std::unique_ptr<ShaderNode> > falseBranchCopy;
        falseBranchCopy.reserve(falseBranch.size());
        for (auto &node: falseBranch) {
            falseBranchCopy.push_back(node->copy());
        }
        return std::make_unique<NodeBranch>(condition->copy(),
                                            std::move(trueBranchCopy),
                                            std::move(falseBranchCopy));
    }

    std::unique_ptr<ShaderNode> loop(const std::unique_ptr<ShaderNode> &initializer,
                                     const std::unique_ptr<ShaderNode> &predicate,
                                     const std::unique_ptr<ShaderNode> &iterator,
                                     const std::vector<std::unique_ptr<ShaderNode> > &body) {
        std::vector<std::unique_ptr<ShaderNode> > bodyCopy;
        bodyCopy.reserve(body.size());
        for (auto &node: body) {
            bodyCopy.push_back(node->copy());
        }
        return std::make_unique<NodeLoop>(initializer ? initializer->copy() : nullptr,
                                          predicate ? predicate->copy() : nullptr,
                                          iterator ? iterator->copy() : nullptr,
                                          std::move(bodyCopy));
    }
}
