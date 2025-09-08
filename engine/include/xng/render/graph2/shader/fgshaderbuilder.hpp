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

#include <utility>

#include "xng/render/graph2/shader/fgshaderliteral.hpp"
#include "xng/render/graph2/shader/fgshadernode.hpp"
#include "xng/render/graph2/shader/fgshadersource.hpp"

namespace xng {
    class XENGINE_EXPORT FGShaderBuilder {
    public:
        std::shared_ptr<FGShaderNode> literal(const FGShaderLiteral &value);

        std::shared_ptr<FGShaderNode> vector(const std::shared_ptr<FGShaderNode> &x,
                                             const std::shared_ptr<FGShaderNode> &y,
                                             const std::shared_ptr<FGShaderNode> &z,
                                             const std::shared_ptr<FGShaderNode> &w);

        std::shared_ptr<FGShaderNode> attributeRead(int32_t attributeIndex);

        std::shared_ptr<FGShaderNode> attributeWrite(int32_t attributeIndex,
                                                     const std::shared_ptr<FGShaderNode> &value);

        std::shared_ptr<FGShaderNode> parameterRead(const std::string &parameter_name);

        std::shared_ptr<FGShaderNode> textureSample(const std::string &textureName,
                                                    const std::shared_ptr<FGShaderNode> &coordinate,
                                                    const std::shared_ptr<FGShaderNode> &bias);

        std::shared_ptr<FGShaderNode> textureSize(const std::string &textureName);

        std::shared_ptr<FGShaderNode> bufferRead(const std::string &bufferName, const std::string &elementName);

        std::shared_ptr<FGShaderNode> bufferWrite(const std::string &bufferName,
                                                  const std::string &elementName,
                                                  const std::shared_ptr<FGShaderNode> &value);

        std::shared_ptr<FGShaderNode> add(const std::shared_ptr<FGShaderNode> &left,
                                          const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> subtract(const std::shared_ptr<FGShaderNode> &left,
                                               const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> multiply(const std::shared_ptr<FGShaderNode> &left,
                                               const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> divide(const std::shared_ptr<FGShaderNode> &left,
                                             const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> compareEqual(const std::shared_ptr<FGShaderNode> &left,
                                                   const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> compareNotEqual(const std::shared_ptr<FGShaderNode> &left,
                                                      const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> compareGreater(const std::shared_ptr<FGShaderNode> &left,
                                                     const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> compareLess(const std::shared_ptr<FGShaderNode> &left,
                                                  const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> compareGreaterEqual(const std::shared_ptr<FGShaderNode> &left,
                                                          const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> compareLessEqual(const std::shared_ptr<FGShaderNode> &left,
                                                       const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> logicalAnd(const std::shared_ptr<FGShaderNode> &left,
                                                 const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> logicalOr(const std::shared_ptr<FGShaderNode> &left,
                                                const std::shared_ptr<FGShaderNode> &right);

        std::shared_ptr<FGShaderNode> normalize(const std::shared_ptr<FGShaderNode> &value);

        std::shared_ptr<FGShaderNode> subscript(const std::shared_ptr<FGShaderNode> &value,
                                                const std::shared_ptr<FGShaderNode> &row,
                                                const std::shared_ptr<FGShaderNode> &column = nullptr);

        std::shared_ptr<FGShaderNode> branch(const std::shared_ptr<FGShaderNode> &condition,
                                             const std::shared_ptr<FGShaderNode> &trueBranch,
                                             const std::shared_ptr<FGShaderNode> &falseBranch);

        std::shared_ptr<FGShaderNode> loop(const std::shared_ptr<FGShaderNode> &iterationStart,
                                           const std::shared_ptr<FGShaderNode> &iterationEnd,
                                           const std::shared_ptr<FGShaderNode> &iterationStep);

        std::shared_ptr<FGShaderNode> getX(const std::shared_ptr<FGShaderNode> &value) {
            return subscript(value, literal(0));
        }

        std::shared_ptr<FGShaderNode> getY(const std::shared_ptr<FGShaderNode> &value) {
            return subscript(value, literal(1));
        }

        std::shared_ptr<FGShaderNode> getZ(const std::shared_ptr<FGShaderNode> &value) {
            return subscript(value, literal(2));
        }

        std::shared_ptr<FGShaderNode> getW(const std::shared_ptr<FGShaderNode> &value) {
            return subscript(value, literal(3));
        }

        FGShaderSource build(FGShaderSource::ShaderStage stage,
                             const FGAttributeLayout &inputLayout,
                             const FGAttributeLayout &outputLayout,
                             const std::unordered_map<std::string, FGShaderValue> &parameters,
                             const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                             const std::unordered_map<std::string, FGTexture> &textures);

    private:
        static void connectNodes(FGShaderNodeInput &targetInput,
                                 const std::shared_ptr<FGShaderNode> &target,
                                 const std::shared_ptr<FGShaderNode> &source);

        std::vector<std::shared_ptr<FGShaderNode> > nodes;
    };
}

#endif //XENGINE_FGSHADERBUILDER_HPP
