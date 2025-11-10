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

#ifndef XENGINE_SHADER_HPP
#define XENGINE_SHADER_HPP

#include <unordered_map>
#include <utility>

#include "xng/rendergraph/renderprimitive.hpp"
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

        RenderPrimitive geometryInput{}; // The input primitive for geometry shaders. Must match the pipeline primitive.
        RenderPrimitive geometryOutput{}; // The output primitive for geometry shaders
        size_t geometryMaxVertices{}; // The maximum number of output vertices for geometry shaders.

        ShaderAttributeLayout inputLayout;
        ShaderAttributeLayout outputLayout;

        std::unordered_map<std::string, ShaderPrimitiveType> parameters{};
        std::unordered_map<std::string, ShaderBuffer> buffers{};
        std::unordered_map<std::string, ShaderTextureArray> textureArrays{};

        std::vector<ShaderStruct> typeDefinitions{};

        std::vector<ShaderInstruction> mainFunction{};

        /**
         * The available functions.
         * Functions can be overloaded by specifying multiple functions with the same name but different argument signatures.
         */
        std::vector<ShaderFunction> functions{};

        Shader() = default;

        Shader(const Stage stage,
               RenderPrimitive geometry_input,
               RenderPrimitive geometry_output,
               size_t geometry_max_vertices,
               ShaderAttributeLayout input_layout,
               ShaderAttributeLayout output_layout,
               const std::unordered_map<std::string, ShaderPrimitiveType> &parameters,
               const std::unordered_map<std::string, ShaderBuffer> &buffers,
               const std::unordered_map<std::string, ShaderTextureArray> &textureArrays,
               const std::vector<ShaderStruct> &typeDefinitions,
               std::vector<ShaderInstruction> mainFunction,
               std::vector<ShaderFunction> functions)
            : stage(stage),
              geometryInput(geometry_input),
              geometryOutput(geometry_output),
              geometryMaxVertices(geometry_max_vertices),
              inputLayout(std::move(input_layout)),
              outputLayout(std::move(output_layout)),
              parameters(parameters),
              buffers(buffers),
              textureArrays(textureArrays),
              typeDefinitions(typeDefinitions),
              mainFunction(std::move(mainFunction)),
              functions(std::move(functions)) {
        }

        Shader(const Shader &other) = default;

        Shader &operator=(const Shader &other) = default;

        Shader(Shader &&other) noexcept = default;

        Shader &operator=(Shader &&other) noexcept = default;
    };
}

#endif //XENGINE_SHADER_HPP
