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

#ifndef XENGINE_SHADERSTAGE_HPP
#define XENGINE_SHADERSTAGE_HPP

#include <unordered_map>
#include <utility>

#include "xng/rendergraph/shader/shadertexture.hpp"
#include "xng/rendergraph/shader/shaderattributelayout.hpp"
#include "xng/rendergraph/shader/shaderfunction.hpp"
#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shaderbuffer.hpp"

namespace xng {
    struct ShaderBuffer;

    /**
     * The shader compiler generates shader source code from the data in ShaderStage.
     */
    struct ShaderStage {
        enum Type {
            VERTEX = 0,
            GEOMETRY,
            TESSELATION_CONTROL,
            TESSELATION_EVALUATION,
            FRAGMENT,
            COMPUTE,
        } type{};

        ShaderAttributeLayout inputLayout;
        ShaderAttributeLayout outputLayout;

        std::unordered_map<std::string, ShaderDataType> parameters;
        std::unordered_map<std::string, ShaderBuffer> buffers;
        std::vector<ShaderTexture> textures; // The available textures in binding order

        std::vector<std::unique_ptr<ShaderNode> > mainFunction;
        std::unordered_map<std::string, ShaderFunction> functions;

        ShaderStage() = default;

        ShaderStage(const Type stage,
                    ShaderAttributeLayout input_layout,
                    ShaderAttributeLayout output_layout,
                    const std::unordered_map<std::string, ShaderDataType> &parameters,
                    const std::unordered_map<std::string, ShaderBuffer> &buffers,
                    const std::vector<ShaderTexture> &textures,
                    std::vector<std::unique_ptr<ShaderNode> > mainFunction,
                    std::unordered_map<std::string, ShaderFunction> functions)
            : type(stage),
              inputLayout(std::move(input_layout)),
              outputLayout(std::move(output_layout)),
              parameters(parameters),
              buffers(buffers),
              textures(textures),
              mainFunction(std::move(mainFunction)),
              functions(std::move(functions)) {
        }

        ShaderStage(const ShaderStage &other)
            : type(other.type),
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

        ShaderStage(ShaderStage &&other) noexcept
            : type(other.type),
              inputLayout(std::move(other.inputLayout)),
              outputLayout(std::move(other.outputLayout)),
              parameters(std::move(other.parameters)),
              buffers(std::move(other.buffers)),
              textures(std::move(other.textures)),
              mainFunction(std::move(other.mainFunction)),
              functions(std::move(other.functions)) {
        }

        ShaderStage &operator=(const ShaderStage &other) {
            if (this == &other)
                return *this;
            type = other.type;
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

        ShaderStage &operator=(ShaderStage &&other) noexcept {
            if (this == &other)
                return *this;
            type = other.type;
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

#endif //XENGINE_SHADERSTAGE_HPP
