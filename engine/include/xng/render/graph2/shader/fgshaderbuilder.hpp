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
    class FGShaderBuilder {
    public:
        std::shared_ptr<FGShaderNode> literal(FGShaderLiteral value);

        std::shared_ptr<FGShaderNode> vector(std::shared_ptr<FGShaderNode> x,
                                             std::shared_ptr<FGShaderNode> y,
                                             std::shared_ptr<FGShaderNode> z,
                                             std::shared_ptr<FGShaderNode> w);

        std::shared_ptr<FGShaderNode> attributeRead(int32_t attributeIndex);

        std::shared_ptr<FGShaderNode> attributeWrite(int32_t attributeIndex, std::shared_ptr<FGShaderNode> value);

        std::shared_ptr<FGShaderNode> parameterRead(std::string parameter_name);

        std::shared_ptr<FGShaderNode> parameterWrite(std::string parameter_name, std::shared_ptr<FGShaderNode> value);

        std::shared_ptr<FGShaderNode> textureSample(std::string textureName,
                                                    std::shared_ptr<FGShaderNode> coordinate,
                                                    std::shared_ptr<FGShaderNode> bias);

        std::shared_ptr<FGShaderNode> add(std::shared_ptr<FGShaderNode> left, std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> subtract(std::shared_ptr<FGShaderNode> left, std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> multiply(std::shared_ptr<FGShaderNode> left, std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> divide(std::shared_ptr<FGShaderNode> left, std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> compareEqual(std::shared_ptr<FGShaderNode> left,
                                                   std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> compareNotEqual(std::shared_ptr<FGShaderNode> left,
                                                      std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> compareGreater(std::shared_ptr<FGShaderNode> left,
                                                     std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> compareLess(std::shared_ptr<FGShaderNode> left,
                                                  std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> compareGreaterEqual(std::shared_ptr<FGShaderNode> left,
                                                          std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> compareLessEqual(std::shared_ptr<FGShaderNode> left,
                                                       std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> logicalAnd(std::shared_ptr<FGShaderNode> left,
                                                 std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> logicalOr(std::shared_ptr<FGShaderNode> left,
                                                std::shared_ptr<FGShaderNode> right);

        std::shared_ptr<FGShaderNode> normalize(std::shared_ptr<FGShaderNode> value);

        std::shared_ptr<FGShaderNode> subscript(std::shared_ptr<FGShaderNode> value,
                                                std::shared_ptr<FGShaderNode> row,
                                                std::shared_ptr<FGShaderNode> column = nullptr);

        std::shared_ptr<FGShaderNode> branch(std::shared_ptr<FGShaderNode> condition,
                                             std::shared_ptr<FGShaderNode> trueBranch,
                                             std::shared_ptr<FGShaderNode> falseBranch);

        std::shared_ptr<FGShaderNode> loop(std::shared_ptr<FGShaderNode> iterationStart,
                                           std::shared_ptr<FGShaderNode> iterationEnd,
                                           std::shared_ptr<FGShaderNode> iterationStep);

        std::shared_ptr<FGShaderNode> getX(std::shared_ptr<FGShaderNode> value) {
            return subscript(std::move(value), literal(0));
        }

        std::shared_ptr<FGShaderNode> getY(std::shared_ptr<FGShaderNode> value) {
            return subscript(std::move(value), literal(1));
        }

        std::shared_ptr<FGShaderNode> getZ(std::shared_ptr<FGShaderNode> value) {
            return subscript(std::move(value), literal(2));
        }

        std::shared_ptr<FGShaderNode> getW(std::shared_ptr<FGShaderNode> value) {
            return subscript(std::move(value), literal(3));
        }

        FGShaderSource build(FGShaderSource::ShaderStage stage,
                             FGAttributeLayout inputLayout,
                             FGAttributeLayout outputLayout);

    private:
        std::vector<std::shared_ptr<FGShaderNode> > nodes;
    };
}

#endif //XENGINE_FGSHADERBUILDER_HPP
