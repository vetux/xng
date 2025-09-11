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

#ifndef XENGINE_FGSHADERSOURCE_HPP
#define XENGINE_FGSHADERSOURCE_HPP

#include <unordered_map>

#include "xng/render/graph2/shader/fgattributelayout.hpp"
#include "xng/render/graph2/shader/fgshaderfunction.hpp"
#include "xng/render/graph2/texture/fgtexture.hpp"
#include "xng/render/graph2/shader/fgshadervalue.hpp"
#include "xng/render/graph2/shader/fgshaderbuffer.hpp"

namespace xng {
    struct FGShaderBuffer;

    /**
     * The shader compiler generates shader source code from the data in FGShaderSource.
     */
    class FGShaderSource {
    public:
        enum ShaderStage {
            VERTEX,
            GEOMETRY,
            TESSELATION_CONTROL,
            TESSELATION_EVALUATION,
            FRAGMENT,
            COMPUTE,
        } stage;

        FGAttributeLayout inputLayout;
        FGAttributeLayout outputLayout;

        std::unordered_map<std::string, FGShaderValue> parameters;
        std::unordered_map<std::string, FGShaderBuffer> buffers;
        std::unordered_map<std::string, FGTexture> textures;

        std::vector<std::unique_ptr<FGShaderNode> > mainFunction;
        std::unordered_map<std::string, FGShaderFunction> functions;

        FGShaderSource() = default;

        FGShaderSource(const ShaderStage stage,
                       const FGAttributeLayout &input_layout,
                       const FGAttributeLayout &output_layout,
                       const std::unordered_map<std::string, FGShaderValue> &parameters,
                       const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                       const std::unordered_map<std::string, FGTexture> &textures,
                       std::vector<std::unique_ptr<FGShaderNode> > mainFunction,
                       std::unordered_map<std::string, FGShaderFunction> functions)
            : stage(stage),
              inputLayout(input_layout),
              outputLayout(output_layout),
              parameters(parameters),
              buffers(buffers),
              textures(textures),
              mainFunction(std::move(mainFunction)),
              functions(std::move(functions)) {
        }

        FGShaderSource(const FGShaderSource &other)
            : stage(other.stage),
              inputLayout(other.inputLayout),
              outputLayout(other.outputLayout),
              parameters(other.parameters),
              buffers(other.buffers),
              textures(other.textures),
              functions(other.functions) {
            for (auto &node: other.mainFunction) {
                mainFunction.push_back(node->copy());
            }
        }

        FGShaderSource(FGShaderSource &&other) noexcept
            : stage(other.stage),
              inputLayout(std::move(other.inputLayout)),
              outputLayout(std::move(other.outputLayout)),
              parameters(std::move(other.parameters)),
              buffers(std::move(other.buffers)),
              textures(std::move(other.textures)),
              mainFunction(std::move(other.mainFunction)),
              functions(std::move(other.functions)) {
        }

        FGShaderSource &operator=(const FGShaderSource &other) {
            if (this == &other)
                return *this;
            stage = other.stage;
            inputLayout = other.inputLayout;
            outputLayout = other.outputLayout;
            parameters = other.parameters;
            buffers = other.buffers;
            textures = other.textures;
            functions = other.functions;
            for (auto &node: other.mainFunction) {
                mainFunction.push_back(node->copy());
            }
            return *this;
        }

        FGShaderSource &operator=(FGShaderSource &&other) noexcept {
            if (this == &other)
                return *this;
            stage = other.stage;
            inputLayout = std::move(other.inputLayout);
            outputLayout = std::move(other.outputLayout);
            parameters = std::move(other.parameters);
            buffers = std::move(other.buffers);
            textures = std::move(other.textures);
            mainFunction = std::move(other.mainFunction);
            functions = std::move(other.functions);
            return *this;
        }
    };
}
#endif //XENGINE_FGSHADERSOURCE_HPP
