/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RENDERGRAPH_SHADER_HPP
#define XENGINE_RENDERGRAPH_SHADER_HPP

#include <unordered_map>
#include <utility>

#include "xng/rendergraph/primitive.hpp"
#include "xng/rendergraph/shader/shaderattributelayout.hpp"
#include "xng/rendergraph/shader/shaderfunction.hpp"
#include "xng/rendergraph/shader/shaderdatatype.hpp"
#include "xng/rendergraph/shader/shaderstoragebuffer.hpp"
#include "xng/rendergraph/shader/shaderuniformbuffer.hpp"
#include "xng/rendergraph/shader/shadertexturearray.hpp"
#include "xng/rendergraph/shader/shaderstructtype.hpp"

namespace xng::rg {
    struct Shader {
        enum Stage {
            VERTEX = 0,
            GEOMETRY,
            TESSELATION_CONTROL,
            TESSELATION_EVALUATION,
            FRAGMENT,
            COMPUTE,
        } stage{};

        Primitive geometryInput{}; // The input primitive for geometry shaders. Must match the pipeline primitive.
        Primitive geometryOutput{}; // The output primitive for geometry shaders
        size_t geometryMaxVertices{}; // The maximum number of output vertices for geometry shaders.

        // The workgroup size of a compute shader.
        // When calling ComputeContext.dispatch(numberOfWorkGroups) each workgroup runs computeLocalSize threads.
        Vec3u computeLocalSize = {1, 1, 1};

        ShaderAttributeLayout inputLayout;
        ShaderAttributeLayout outputLayout;

        std::unordered_map<std::string, ShaderPrimitiveType> parameters{};

        std::unordered_map<std::string, ShaderUniformBuffer> uniformBuffers{};
        std::unordered_map<std::string, ShaderStorageBuffer> storageBuffers{};

        std::unordered_map<std::string, ShaderTextureArray> textureArrays{};

        std::vector<ShaderStructType> typeDefinitions{};

        std::vector<ShaderInstruction> mainFunction{};

        /**
         * The available functions.
         * Functions can be overloaded by specifying multiple functions with the same name but different argument signatures.
         */
        std::vector<ShaderFunction> functions{};

        Shader() = default;

        Shader(const Stage stage,
               const Primitive geometry_input,
               const Primitive geometry_output,
               const size_t geometry_max_vertices,
               Vec3u compute_local_size,
               ShaderAttributeLayout input_layout,
               ShaderAttributeLayout output_layout,
               std::unordered_map<std::string, ShaderPrimitiveType> parameters,
               std::unordered_map<std::string, ShaderUniformBuffer> uniformBuffers,
               std::unordered_map<std::string, ShaderStorageBuffer> storageBuffers,
               std::unordered_map<std::string, ShaderTextureArray> textureArrays,
               std::vector<ShaderStructType> typeDefinitions,
               std::vector<ShaderInstruction> mainFunction,
               std::vector<ShaderFunction> functions)
            : stage(stage),
              geometryInput(geometry_input),
              geometryOutput(geometry_output),
              geometryMaxVertices(geometry_max_vertices),
              computeLocalSize(std::move(compute_local_size)),
              inputLayout(std::move(input_layout)),
              outputLayout(std::move(output_layout)),
              parameters(std::move(parameters)),
              uniformBuffers(std::move(uniformBuffers)),
              storageBuffers(std::move(storageBuffers)),
              textureArrays(std::move(textureArrays)),
              typeDefinitions(std::move(typeDefinitions)),
              mainFunction(std::move(mainFunction)),
              functions(std::move(functions)) {
        }

        Shader(const Shader &other) = default;

        Shader &operator=(const Shader &other) = default;

        Shader(Shader &&other) noexcept = default;

        Shader &operator=(Shader &&other) noexcept = default;
    };
}

#endif //XENGINE_RENDERGRAPH_SHADER_HPP
