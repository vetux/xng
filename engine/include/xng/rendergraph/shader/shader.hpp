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

#ifndef XENGINE_SHADER_HPP
#define XENGINE_SHADER_HPP

#include <unordered_map>
#include <utility>

#include "xng/rendergraph/shader/shaderattributelayout.hpp"
#include "xng/rendergraph/shader/shaderfunction.hpp"
#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shaderbuffer.hpp"
#include "xng/rendergraph/shader/shadertexturearray.hpp"

namespace xng {
    struct ShaderBuffer;

    struct Shader {
        enum Stage {
            VERTEX = 0,
            GEOMETRY,
            TESSELATION_CONTROL,
            TESSELATION_EVALUATION,
            FRAGMENT,
            COMPUTE,
        } stage{};

        ShaderAttributeLayout inputLayout;
        ShaderAttributeLayout outputLayout;

        std::unordered_map<std::string, ShaderDataType> parameters;
        std::unordered_map<std::string, ShaderBuffer> buffers;
        std::unordered_map<std::string, ShaderTextureArray> textureArrays;

        std::vector<std::unique_ptr<ShaderNode> > mainFunction;

        /**
         * The available functions.
         * Functions can be overloaded by specifying multiple functions with the same name but different argument signatures.
         */
        std::vector<ShaderFunction> functions;

        Shader() = default;

        Shader(const Stage stage,
                    ShaderAttributeLayout input_layout,
                    ShaderAttributeLayout output_layout,
                    const std::unordered_map<std::string, ShaderDataType> &parameters,
                    const std::unordered_map<std::string, ShaderBuffer> &buffers,
                    const std::unordered_map<std::string, ShaderTextureArray> &textureArrays,
                    std::vector<std::unique_ptr<ShaderNode> > mainFunction,
                    std::vector<ShaderFunction> functions)
            : stage(stage),
              inputLayout(std::move(input_layout)),
              outputLayout(std::move(output_layout)),
              parameters(parameters),
              buffers(buffers),
              textureArrays(textureArrays),
              mainFunction(std::move(mainFunction)),
              functions(std::move(functions)) {
        }

        Shader(const Shader &other)
            : stage(other.stage),
              inputLayout(other.inputLayout),
              outputLayout(other.outputLayout),
              parameters(other.parameters),
              buffers(other.buffers),
              textureArrays(other.textureArrays),
              functions(other.functions) {
            for (auto &node: other.mainFunction) {
                mainFunction.push_back(node->copy());
            }
        }

        Shader(Shader &&other) noexcept
            : stage(other.stage),
              inputLayout(std::move(other.inputLayout)),
              outputLayout(std::move(other.outputLayout)),
              parameters(std::move(other.parameters)),
              buffers(std::move(other.buffers)),
              textureArrays(std::move(other.textureArrays)),
              mainFunction(std::move(other.mainFunction)),
              functions(std::move(other.functions)) {
        }

        Shader &operator=(const Shader &other) {
            if (this == &other)
                return *this;
            stage = other.stage;
            inputLayout = other.inputLayout;
            outputLayout = other.outputLayout;
            parameters = other.parameters;
            buffers = other.buffers;
            textureArrays = other.textureArrays;
            functions = other.functions;
            for (auto &node: other.mainFunction) {
                mainFunction.push_back(node->copy());
            }
            return *this;
        }

        Shader &operator=(Shader &&other) noexcept {
            if (this == &other)
                return *this;
            stage = other.stage;
            inputLayout = std::move(other.inputLayout);
            outputLayout = std::move(other.outputLayout);
            parameters = std::move(other.parameters);
            buffers = std::move(other.buffers);
            textureArrays = std::move(other.textureArrays);
            mainFunction = std::move(other.mainFunction);
            functions = std::move(other.functions);
            return *this;
        }
    };
}

#endif //XENGINE_SHADER_HPP
