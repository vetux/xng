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

#include "graphics/renderpipeline.hpp"
#include "graphics/rendertarget.hpp"
#include "graphics/texturebuffer.hpp"
#include "graphics/meshbuffer.hpp"

#include "graphics/vertexattribute.hpp"

#include "graphics/renderpipelinedesc.hpp"
#include "graphics/shaderprogramdesc.hpp"
#include "graphics/shaderbufferdesc.hpp"
#include "graphics/meshbufferdesc.hpp"
#include "graphics/rendertargetdesc.hpp"
#include "graphics/texturebufferdesc.hpp"

#include "asset/mesh.hpp"

namespace xengine {
    class XENGINE_EXPORT RenderDevice : public RenderObject {
    public:
        ~RenderDevice() override = default;

        virtual std::unique_ptr<RenderPipeline> createPipeline(RenderPipelineDesc &) = 0;

        virtual std::unique_ptr<RenderPipeline> createPipeline(const uint8_t *cacheData, size_t size) = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) = 0;

        virtual std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) = 0;

        virtual std::unique_ptr<MeshBuffer> createMeshBuffer(const MeshBufferDesc &desc) = 0;

        virtual std::unique_ptr<ShaderProgram> createShaderProgram(const ShaderProgramDesc &desc) = 0;

        virtual std::unique_ptr<ShaderBuffer> createShaderBuffer(const ShaderBufferDesc &desc) = 0;

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
            const std::vector<VertexAttribute> layout = {
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

            const MeshBufferDesc desc = {
                    .vertexLayout = layout,
                    .numberOfVertices = mesh.vertices.size()
            };
            auto ret = createMeshBuffer(desc);
            ret->upload(mesh);
            return ret;
        }

        virtual std::unique_ptr<MeshBuffer> createInstancedMeshBuffer(const Mesh &mesh,
                                                                      const std::vector<Transform> &offsets) {
            const std::vector<VertexAttribute> layout = {
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

            const std::vector<VertexAttribute> instanceLayout = {
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT)
            };

            const MeshBufferDesc desc = {
                    .vertexLayout = layout,
                    .instanceLayout = instanceLayout,
                    .numberOfVertices = mesh.vertices.size(),
                    .numberOfInstances = offsets.size()
            };
        }
    };
}

#endif //XENGINE_RENDERDEVICE_HPP
