/**
 *  xEngine - C++ game engine library
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

#ifndef XENGINE_RENDERDEVICE_HPP
#define XENGINE_RENDERDEVICE_HPP

#include <map>
#include <functional>

#include "graphics/renderobject.hpp"
#include "graphics/renderpipeline.hpp"
#include "graphics/renderpipelinedesc.hpp"
#include "graphics/renderproperties.hpp"
#include "graphics/vertexattribute.hpp"
#include "graphics/spirvsource.hpp"

#include "asset/mesh.hpp"

namespace xengine {
    class XENGINE_EXPORT RenderDevice : public RenderObject {
    public:
        ~RenderDevice() override = default;

        virtual std::unique_ptr<RenderPipeline> createPipeline(RenderPipelineDesc &) = 0;

        virtual std::unique_ptr<RenderPipeline> createPipeline(const uint8_t *cacheData, size_t size) = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(Vec2i size) = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(Vec2i size, int samples) = 0;

        virtual std::unique_ptr<TextureBuffer> createTextureBuffer(TextureBuffer::Attributes attributes) = 0;

        virtual std::unique_ptr<MeshBuffer> createMeshBuffer(const std::vector<VertexAttribute> &layout,
                                                             const uint8_t *buffer,
                                                             size_t numberOfVertices,
                                                             const std::vector<uint> &indices) = 0;

        virtual std::unique_ptr<MeshBuffer> createInstancedMeshBuffer(const std::vector<VertexAttribute> &layout,
                                                                      const uint8_t *buffer,
                                                                      const std::vector<VertexAttribute> &instanceLayout,
                                                                      const uint8_t *instanceBuffer,
                                                                      size_t numberOfVertices,
                                                                      size_t numberOfInstances,
                                                                      const std::vector<uint> &indices) = 0;

        /**
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
        virtual std::unique_ptr<MeshBuffer> createMeshBuffer(const Mesh &mesh) {
            static const std::vector<VertexAttribute> layout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT)
            };
            return createMeshBuffer(layout,
                                    reinterpret_cast<const uint8_t *>(mesh.vertices.data()),
                                    mesh.vertices.size(),
                                    mesh.indices);
        }

        virtual std::unique_ptr<MeshBuffer> createInstancedMeshBuffer(const Mesh &mesh,
                                                                      const std::vector<Transform> &offsets) {
            static const std::vector<VertexAttribute> layout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT)
            };

            static const std::vector<VertexAttribute> instanceLayout = {
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT)
            };

            return createInstancedMeshBuffer(layout,
                                             reinterpret_cast<const uint8_t *>(mesh.vertices.data()),
                                             instanceLayout,
                                             reinterpret_cast<const uint8_t *>(offsets.data()),
                                             mesh.vertices.size(),
                                             offsets.size(),
                                             mesh.indices);
        }

        /**
         * Create a shader program instance for the given shader sources in SPIRV.
         *
         * The implementation may cross compile the spirv to a different language using the ShaderCompiler interface.
         *
         * The shaders are required to store all global variables in uniform buffers.
         * The bindings of uniform buffers and samplers have to be specified in the source.
         *
         *  The shaders specify the layout of the uniform buffers and users have to ensure that
         *  layout of data passed to createShaderBuffer matches the layout specified in the shader.
         *
         *  The shaders specify the layout of the vertex input data and users have to ensure that
         *  the layout of the mesh buffers matches the layout specified in the vertex shader.
         *
         * @param vertexShader
         * @param vertexShaderEntryPoint
         * @param fragmentShader
         * @param fragmentShaderEntryPoint
         * @return
         */
        virtual std::unique_ptr<ShaderProgram> createShaderProgram(const SPIRVSource &vertexShader,
                                                                   const SPIRVSource &fragmentShader) = 0;

        virtual std::unique_ptr<ShaderProgram> createShaderProgram(const SPIRVSource &vertexShader,
                                                                   const SPIRVSource &fragmentShader,
                                                                   const SPIRVSource &geometryShader) = 0;

        virtual std::unique_ptr<ShaderProgram> createShaderProgram(const SPIRVSource &vertexShader,
                                                                   const SPIRVSource &fragmentShader,
                                                                   const SPIRVSource &geometryShader,
                                                                   const SPIRVSource &tessellationShader) = 0;

        virtual std::unique_ptr<ShaderBuffer> createShaderBuffer(size_t size) = 0;
    };
}

#endif //XENGINE_RENDERDEVICE_HPP
