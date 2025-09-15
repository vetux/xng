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

#ifndef XENGINE_SHADERNODEFACTORY_HPP
#define XENGINE_SHADERNODEFACTORY_HPP

#include "xng/rendergraph/shader/shadernode.hpp"
#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shaderliteral.hpp"
#include "xng/rendergraph/shader/nodes/nodevectorswizzle.hpp"

namespace xng {
    namespace ShaderNodeFactory {
        XENGINE_EXPORT std::unique_ptr<ShaderNode> createVariable(const std::string &name,
                                                                  const ShaderDataType &type,
                                                                  const std::unique_ptr<ShaderNode> &value =
                                                                          nullptr,
                                                                  size_t count = 1);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> assign(const std::unique_ptr<ShaderNode> &target,
                                                          const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> variable(const std::string &name);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> literal(const ShaderLiteral &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> argument(const std::string &name);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> attributeInput(uint32_t attributeIndex);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> attributeOutput(uint32_t attributeIndex);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> parameter(const std::string &parameter_name);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> vertexPosition(const std::unique_ptr<ShaderNode> &position);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> vector(ShaderDataType type,
                                                          const std::unique_ptr<ShaderNode> &x,
                                                          const std::unique_ptr<ShaderNode> &y,
                                                          const std::unique_ptr<ShaderNode> &z = nullptr,
                                                          const std::unique_ptr<ShaderNode> &w = nullptr);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> array(ShaderDataType elementType,
                                                         const std::vector<std::unique_ptr<ShaderNode> > &elements);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> texture(std::string textureName,
                                                           const std::unique_ptr<ShaderNode> &textureIndex = nullptr);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> textureSample(const std::unique_ptr<ShaderNode> &texture,
                                                                 const std::unique_ptr<ShaderNode> &coordinate,
                                                                 const std::unique_ptr<ShaderNode> &lod = nullptr);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> textureSize(const std::unique_ptr<ShaderNode> &texture,
                                                               const std::unique_ptr<ShaderNode> &lod = nullptr);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> textureFetch(const std::unique_ptr<ShaderNode> &texture,
                                                                const std::unique_ptr<ShaderNode> &coordinate,
                                                                const std::unique_ptr<ShaderNode> &index);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> bufferRead(const std::string &bufferName,
                                                              const std::string &elementName,
                                                              const std::unique_ptr<ShaderNode> &index = nullptr);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> bufferWrite(const std::string &bufferName,
                                                               const std::string &elementName,
                                                               const std::unique_ptr<ShaderNode> &index,
                                                               const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> bufferSize(const std::string &bufferName);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> add(const std::unique_ptr<ShaderNode> &left,
                                                       const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> subtract(const std::unique_ptr<ShaderNode> &left,
                                                            const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> multiply(const std::unique_ptr<ShaderNode> &left,
                                                            const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> divide(const std::unique_ptr<ShaderNode> &left,
                                                          const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> compareEqual(const std::unique_ptr<ShaderNode> &left,
                                                                const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> compareNotEqual(const std::unique_ptr<ShaderNode> &left,
                                                                   const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> compareGreater(const std::unique_ptr<ShaderNode> &left,
                                                                  const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> compareLess(const std::unique_ptr<ShaderNode> &left,
                                                               const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> compareGreaterEqual(
            const std::unique_ptr<ShaderNode> &left,
            const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> compareLessEqual(const std::unique_ptr<ShaderNode> &left,
                                                                    const std::unique_ptr<ShaderNode> &
                                                                    right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> logicalAnd(const std::unique_ptr<ShaderNode> &left,
                                                              const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> logicalOr(const std::unique_ptr<ShaderNode> &left,
                                                             const std::unique_ptr<ShaderNode> &right);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> call(const std::string &functionName,
                                                        const std::vector<std::unique_ptr<ShaderNode> > &
                                                        arguments);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> ret(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> abs(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> sin(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> cos(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> tan(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> asin(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> acos(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> atan(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> pow(const std::unique_ptr<ShaderNode> &base,
                                                       const std::unique_ptr<ShaderNode> &exponent);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> exp(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> log(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> sqrt(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> inverseSqrt(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> floor(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> ceil(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> round(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> fract(const std::unique_ptr<ShaderNode> &value);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> mod(const std::unique_ptr<ShaderNode> &x,
                                                       const std::unique_ptr<ShaderNode> &y);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> min(const std::unique_ptr<ShaderNode> &x,
                                                       const std::unique_ptr<ShaderNode> &y);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> max(const std::unique_ptr<ShaderNode> &x,
                                                       const std::unique_ptr<ShaderNode> &y);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> clamp(const std::unique_ptr<ShaderNode> &x,
                                                         const std::unique_ptr<ShaderNode> &min,
                                                         const std::unique_ptr<ShaderNode> &max);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> mix(const std::unique_ptr<ShaderNode> &x,
                                                       const std::unique_ptr<ShaderNode> &y,
                                                       const std::unique_ptr<ShaderNode> &a);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> step(const std::unique_ptr<ShaderNode> &edge,
                                                        const std::unique_ptr<ShaderNode> &x);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> smoothstep(const std::unique_ptr<ShaderNode> &edge0,
                                                              const std::unique_ptr<ShaderNode> &edge1,
                                                              const std::unique_ptr<ShaderNode> &x);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> dot(const std::unique_ptr<ShaderNode> &x,
                                                       const std::unique_ptr<ShaderNode> &y);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> cross(const std::unique_ptr<ShaderNode> &x,
                                                         const std::unique_ptr<ShaderNode> &y);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> normalize(const std::unique_ptr<ShaderNode> &x);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> length(const std::unique_ptr<ShaderNode> &x);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> distance(const std::unique_ptr<ShaderNode> &x,
                                                            const std::unique_ptr<ShaderNode> &y);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> reflect(const std::unique_ptr<ShaderNode> &i,
                                                           const std::unique_ptr<ShaderNode> &n);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> refract(const std::unique_ptr<ShaderNode> &i,
                                                           const std::unique_ptr<ShaderNode> &n,
                                                           const std::unique_ptr<ShaderNode> &eta);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> faceforward(const std::unique_ptr<ShaderNode> &n,
                                                               const std::unique_ptr<ShaderNode> &i,
                                                               const std::unique_ptr<ShaderNode> &nref);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> subscriptArray(const std::unique_ptr<ShaderNode> &array,
                                                                  const std::unique_ptr<ShaderNode> &index);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> vectorSwizzle(const std::unique_ptr<ShaderNode> &value,
                                                                 const std::vector<NodeVectorSwizzle::ComponentIndex> &
                                                                 indices);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> subscriptMatrix(const std::unique_ptr<ShaderNode> &matrix,
                                                                   const std::unique_ptr<ShaderNode> &row,
                                                                   const std::unique_ptr<ShaderNode> &column
                                                                           =
                                                                           nullptr);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> branch(const std::unique_ptr<ShaderNode> &condition,
                                                          const std::vector<std::unique_ptr<ShaderNode> > &
                                                          trueBranch,
                                                          const std::vector<std::unique_ptr<ShaderNode> > &
                                                          falseBranch);

        XENGINE_EXPORT std::unique_ptr<ShaderNode> loop(const std::unique_ptr<ShaderNode> &initializer,
                                                        const std::unique_ptr<ShaderNode> &predicate,
                                                        const std::unique_ptr<ShaderNode> &iterator,
                                                        const std::vector<std::unique_ptr<ShaderNode> > &body);

        inline std::unique_ptr<ShaderNode> getX(const std::unique_ptr<ShaderNode> &value) {
            return vectorSwizzle(std::move(value), {NodeVectorSwizzle::COMPONENT_X});
        }

        inline std::unique_ptr<ShaderNode> getY(const std::unique_ptr<ShaderNode> &value) {
            return vectorSwizzle(std::move(value), {NodeVectorSwizzle::COMPONENT_Y});
        }

        inline std::unique_ptr<ShaderNode> getZ(const std::unique_ptr<ShaderNode> &value) {
            return vectorSwizzle(std::move(value), {NodeVectorSwizzle::COMPONENT_Z});
        }

        inline std::unique_ptr<ShaderNode> getW(const std::unique_ptr<ShaderNode> &value) {
            return vectorSwizzle(std::move(value), {NodeVectorSwizzle::COMPONENT_W});
        }
    }
}

#endif //XENGINE_SHADERNODEFACTORY_HPP
