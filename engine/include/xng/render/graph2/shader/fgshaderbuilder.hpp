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

#ifndef XENGINE_FGSHADERBUILDER_HPP
#define XENGINE_FGSHADERBUILDER_HPP

#include <memory>
#include <utility>
#include <memory>
#include <optional>

#include "fgshaderfunction.hpp"
#include "nodes/fgnodevariablewrite.hpp"
#include "xng/render/graph2/shader/fgshaderliteral.hpp"
#include "xng/render/graph2/shader/fgshadernode.hpp"
#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    class XENGINE_EXPORT FGShaderBuilder {
    public:
        std::unique_ptr<FGShaderNode> createVariable(const std::string &name,
                                                     const FGShaderValue &type,
                                                     const std::unique_ptr<FGShaderNode> &value = nullptr);

        std::unique_ptr<FGShaderNode> assignVariable(const std::string &variableName,
                                                     const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> variable(const std::string &name);

        std::unique_ptr<FGShaderNode> literal(const FGShaderLiteral &value);

        std::unique_ptr<FGShaderNode> argument(const std::string &name);

        std::unique_ptr<FGShaderNode> vector(FGShaderValue type,
                                             const std::unique_ptr<FGShaderNode> &x,
                                             const std::unique_ptr<FGShaderNode> &y,
                                             const std::unique_ptr<FGShaderNode> &z = nullptr,
                                             const std::unique_ptr<FGShaderNode> &w = nullptr);

        std::unique_ptr<FGShaderNode> attributeRead(int32_t attributeIndex);

        std::unique_ptr<FGShaderNode> attributeWrite(int32_t attributeIndex,
                                                     const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> parameterRead(std::string parameter_name);

        std::unique_ptr<FGShaderNode> textureSample(const std::string &textureName,
                                                    const std::unique_ptr<FGShaderNode> &coordinate,
                                                    const std::unique_ptr<FGShaderNode> &bias);

        std::unique_ptr<FGShaderNode> textureSize(const std::string &textureName);

        std::unique_ptr<FGShaderNode> bufferRead(const std::string &bufferName,
                                                 const std::string &elementName,
                                                 const std::unique_ptr<FGShaderNode> &index = nullptr);

        std::unique_ptr<FGShaderNode> bufferWrite(const std::string &bufferName,
                                                  const std::string &elementName,
                                                  const std::unique_ptr<FGShaderNode> &value,
                                                  const std::unique_ptr<FGShaderNode> &index = nullptr);

        std::unique_ptr<FGShaderNode> bufferSize(const std::string &bufferName);

        std::unique_ptr<FGShaderNode> add(const std::unique_ptr<FGShaderNode> &left,
                                          const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> subtract(const std::unique_ptr<FGShaderNode> &left,
                                               const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> multiply(const std::unique_ptr<FGShaderNode> &left,
                                               const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> divide(const std::unique_ptr<FGShaderNode> &left,
                                             const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> compareEqual(const std::unique_ptr<FGShaderNode> &left,
                                                   const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> compareNotEqual(const std::unique_ptr<FGShaderNode> &left,
                                                      const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> compareGreater(const std::unique_ptr<FGShaderNode> &left,
                                                     const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> compareLess(const std::unique_ptr<FGShaderNode> &left,
                                                  const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> compareGreaterEqual(const std::unique_ptr<FGShaderNode> &left,
                                                          const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> compareLessEqual(const std::unique_ptr<FGShaderNode> &left,
                                                       const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> logicalAnd(const std::unique_ptr<FGShaderNode> &left,
                                                 const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> logicalOr(const std::unique_ptr<FGShaderNode> &left,
                                                const std::unique_ptr<FGShaderNode> &right);

        std::unique_ptr<FGShaderNode> call(const std::string &functionName,
                                           const std::vector<std::unique_ptr<FGShaderNode> > &arguments);

        std::unique_ptr<FGShaderNode> ret(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> abs(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> sin(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> cos(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> tan(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> asin(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> acos(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> atan(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> pow(const std::unique_ptr<FGShaderNode> &base,
                                          const std::unique_ptr<FGShaderNode> &exponent);

        std::unique_ptr<FGShaderNode> exp(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> log(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> sqrt(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> inverseSqrt(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> floor(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> ceil(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> round(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> fract(const std::unique_ptr<FGShaderNode> &value);

        std::unique_ptr<FGShaderNode> mod(const std::unique_ptr<FGShaderNode> &x,
                                          const std::unique_ptr<FGShaderNode> &y);

        std::unique_ptr<FGShaderNode> min(const std::unique_ptr<FGShaderNode> &x,
                                          const std::unique_ptr<FGShaderNode> &y);

        std::unique_ptr<FGShaderNode> max(const std::unique_ptr<FGShaderNode> &x,
                                          std::unique_ptr<FGShaderNode> &y);

        std::unique_ptr<FGShaderNode> clamp(const std::unique_ptr<FGShaderNode> &x,
                                            const std::unique_ptr<FGShaderNode> &min,
                                            const std::unique_ptr<FGShaderNode> &max);

        std::unique_ptr<FGShaderNode> mix(const std::unique_ptr<FGShaderNode> &x,
                                          const std::unique_ptr<FGShaderNode> &y,
                                          const std::unique_ptr<FGShaderNode> &a);

        std::unique_ptr<FGShaderNode> step(const std::unique_ptr<FGShaderNode> &edge,
                                           const std::unique_ptr<FGShaderNode> &x);

        std::unique_ptr<FGShaderNode> smoothstep(const std::unique_ptr<FGShaderNode> &edge0,
                                                 const std::unique_ptr<FGShaderNode> &edge1,
                                                 const std::unique_ptr<FGShaderNode> &x);

        std::unique_ptr<FGShaderNode> dot(const std::unique_ptr<FGShaderNode> &x,
                                          const std::unique_ptr<FGShaderNode> &y);

        std::unique_ptr<FGShaderNode> cross(const std::unique_ptr<FGShaderNode> &x,
                                            const std::unique_ptr<FGShaderNode> &y);

        std::unique_ptr<FGShaderNode> normalize(const std::unique_ptr<FGShaderNode> &x);

        std::unique_ptr<FGShaderNode> length(const std::unique_ptr<FGShaderNode> &x);

        std::unique_ptr<FGShaderNode> distance(const std::unique_ptr<FGShaderNode> &x,
                                               const std::unique_ptr<FGShaderNode> &y);

        std::unique_ptr<FGShaderNode> reflect(const std::unique_ptr<FGShaderNode> &i,
                                              const std::unique_ptr<FGShaderNode> &n);

        std::unique_ptr<FGShaderNode> refract(const std::unique_ptr<FGShaderNode> &i,
                                              const std::unique_ptr<FGShaderNode> &n,
                                              const std::unique_ptr<FGShaderNode> &eta);

        std::unique_ptr<FGShaderNode> faceforward(const std::unique_ptr<FGShaderNode> &n,
                                                  const std::unique_ptr<FGShaderNode> &i,
                                                  const std::unique_ptr<FGShaderNode> &nref);

        std::unique_ptr<FGShaderNode> subscriptArray(const std::unique_ptr<FGShaderNode> &array,
                                                     const std::unique_ptr<FGShaderNode> &index);

        std::unique_ptr<FGShaderNode> subscriptVector(const std::unique_ptr<FGShaderNode> &value, int index);

        std::unique_ptr<FGShaderNode> subscriptMatrix(const std::unique_ptr<FGShaderNode> &matrix,
                                                      const std::unique_ptr<FGShaderNode> &row,
                                                      const std::unique_ptr<FGShaderNode> &column = nullptr);

        std::unique_ptr<FGShaderNode> branch(const std::unique_ptr<FGShaderNode> &condition,
                                             const std::vector<std::unique_ptr<FGShaderNode> > &trueBranch,
                                             const std::vector<std::unique_ptr<FGShaderNode> > &falseBranch);

        std::unique_ptr<FGShaderNode> loop(const std::unique_ptr<FGShaderNode> &initializer,
                                           const std::unique_ptr<FGShaderNode> &predicate,
                                           const std::unique_ptr<FGShaderNode> &iterator,
                                           const std::vector<std::unique_ptr<FGShaderNode> > &body);

        std::unique_ptr<FGShaderNode> getX(const std::unique_ptr<FGShaderNode> &value) {
            return subscriptVector(std::move(value), 0);
        }

        std::unique_ptr<FGShaderNode> getY(const std::unique_ptr<FGShaderNode> &value) {
            return subscriptVector(std::move(value), 1);
        }

        std::unique_ptr<FGShaderNode> getZ(const std::unique_ptr<FGShaderNode> &value) {
            return subscriptVector(std::move(value), 2);
        }

        std::unique_ptr<FGShaderNode> getW(const std::unique_ptr<FGShaderNode> &value) {
            return subscriptVector(std::move(value), 3);
        }

        void defineFunction(const std::string &name,
                            const std::vector<std::unique_ptr<FGShaderNode> > &body,
                            const std::unordered_map<std::string, FGShaderValue> &arguments,
                            FGShaderValue returnType);

        FGShaderSource build(FGShaderSource::ShaderStage stage,
                             const FGAttributeLayout &inputLayout,
                             const FGAttributeLayout &outputLayout,
                             const std::unordered_map<std::string, FGShaderValue> &parameters,
                             const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                             const std::unordered_map<std::string, FGTexture> &textures,
                             const std::vector<std::unique_ptr<FGShaderNode> > &mainFunction);

    private:
        std::unordered_map<std::string, FGShaderFunction> functions;
    };
}

#endif //XENGINE_FGSHADERBUILDER_HPP
