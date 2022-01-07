/**
 *  XEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERALLOCATOR_HPP
#define XENGINE_RENDERALLOCATOR_HPP

#include <memory>

#include "rendertarget.hpp"
#include "texturebuffer.hpp"
#include "meshbuffer.hpp"
#include "shaderprogram.hpp"
#include "shadersource.hpp"

#include "asset/mesh.hpp"

namespace xengine {
    class XENGINE_EXPORT RenderAllocator {
    public:
        virtual std::unique_ptr<RenderTarget> createRenderTarget(Vec2i size, int samples) = 0;

        virtual std::unique_ptr<TextureBuffer> createTextureBuffer(TextureBuffer::Attributes attributes) = 0;

        /**
         * Create a mesh buffer instance for the given mesh.
         *
         * The mesh buffer vertex attributes required in vertex shaders which draw the mesh buffer are bound as follows:
         *
         * GLSL:
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *  layout (location = 5) in vec4 instanceRow0;
         *  layout (location = 6) in vec4 instanceRow1;
         *  layout (location = 7) in vec4 instanceRow2;
         *  layout (location = 8) in vec4 instanceRow3;
         *
         * HLSL:
         *  struct XENGINE_EXPORT VS_INPUT
         *  {
         *      float3 position : POSITION0;
         *      float3 normal : NORMAL;
         *      float2 uv : TEXCOORD0;
         *      float3 tangent: TANGENT;
         *      float3 bitangent: BINORMAL;
         *      float4 instanceRow0 : POSITION1;
         *      float4 instanceRow1 : POSITION2;
         *      float4 instanceRow2 : POSITION3;
         *      float4 instanceRow3 : POSITION4;
         *  };
         *
         * @param mesh
         * @return
         */
        virtual std::unique_ptr<MeshBuffer> createMeshBuffer(const Mesh &mesh) = 0;

        virtual std::unique_ptr<MeshBuffer> createInstancedMeshBuffer(const Mesh &mesh,
                                                                      const std::vector<Transform> &offsets) = 0;

        struct XENGINE_EXPORT CustomMeshDefinition {
            enum AttributeType {
                UNSIGNED_BYTE, // 1 Byte unsigned
                SIGNED_BYTE, // 1 Byte signed
                UNSIGNED_INT, // 4 Byte unsigned
                SIGNED_INT, // 4 Byte signed
                FLOAT, // 4 Byte float
                DOUBLE // 8 Byte double
            };

            char *data; // A pointer pointing to a buffer containing the mesh data in the specified format.
            size_t dataLength; // The length of the buffer pointed at by the data pointer.
            std::vector<std::pair<int, AttributeType>> vertex; //The count and type of the components of a vertex.
        };

        /**
         * Create a mesh buffer with a configurable source layout.
         *
         * Eg the vertex layout
         * {
         *      { 3, FLOAT },
         *      { 2, UNSIGNED_INT }
         *      { 1, SIGNED_INT }
         * }
         *
         * would give a vertex size of 24 bytes.
         *
         * The data is parsed as (mesh.dataLength / (3 * 4 + 2 * 4 + 1 * 4) vertices.
         *
         * The following input layout could be used when drawing the buffer:
         *
         * GLSL:
         *  layout (location = 0) in vec3 attr0;
         *  layout (location = 1) in vec2ui attr1;
         *  layout (location = 2) in int attr2;
         *
         * @return
         */
        virtual std::unique_ptr<MeshBuffer> createCustomMeshBuffer(const CustomMeshDefinition &mesh) = 0;

        /**
         * Create a shader program instance for the given shader sources.
         *
         * The implementation may cross compile the source to a different language using the ShaderCompiler interface.
         *
         * @param vertexShader
         * @param fragmentShader
         * @return
         */
        virtual std::unique_ptr<ShaderProgram> createShaderProgram(const ShaderSource &vertexShader,
                                                                   const ShaderSource &fragmentShader) = 0;

        virtual std::unique_ptr<ShaderProgram> createShaderProgram(const ShaderSource &vertexShader,
                                                                   const ShaderSource &geometryShader,
                                                                   const ShaderSource &fragmentShader) = 0;
    };
}

#endif //XENGINE_RENDERALLOCATOR_HPP
