/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_RENDERDEVICE_HPP
#define XENGINE_RENDERDEVICE_HPP

#include <map>
#include <functional>

#include "renderpipeline.hpp"
#include "rendercommand.hpp"
#include "xng/gpu/compute/computepipeline.hpp"
#include "xng/gpu/compute/computecommand.hpp"
#include "xng/gpu/raytrace/raytracepipeline.hpp"
#include "xng/gpu/raytrace/raytracecommand.hpp"
#include "rendertarget.hpp"
#include "texturebuffer.hpp"
#include "vertexbuffer.hpp"
#include "shaderprogram.hpp"
#include "shaderbuffer.hpp"
#include "vertexattribute.hpp"
#include "renderpipelinedesc.hpp"
#include "shaderprogramdesc.hpp"
#include "shaderbufferdesc.hpp"
#include "vertexbufferdesc.hpp"
#include "rendertargetdesc.hpp"
#include "texturebufferdesc.hpp"
#include "renderdeviceinfo.hpp"

#include "xng/shader/spirvdecompiler.hpp"

#include "xng/asset/mesh.hpp"

namespace xng {
    /**
     * The render device allocates gpu resources.
     *
     * Smart pointers are used as return type to ensure that user programs are memory safe by default,
     * the user can unbox the returned pointers to use different memory management patterns.
     */
    class XENGINE_EXPORT RenderDevice {
    public:
        /**
         * Deallocates all objects belonging to this device which have not been deallocated yet.
         * Deallocating objects returned by this device after calling this destructor results in undefined behaviour.
         */
        virtual ~RenderDevice() = default;

        virtual const RenderDeviceInfo &getInfo() = 0;

        /**
         * @return The list of currently allocated render objects
         */
        virtual std::set<RenderObject*> getAllocatedObjects() = 0;

        /**
         * The shaders are required to store all global variables in uniform buffers.
         *
         * The bindings of uniform buffers and samplers have to be specified in the source
         * and match the indices of the RenderCommand bindings vector.
         *
         *  The shaders specify the layout of the uniform buffers and users have to ensure that
         *  the layout of data the shader buffers matches the layout specified in the shaders.
         *
         *  The shaders specify the layout of the vertex input data and users have to ensure that
         *  the layout of the mesh buffers matches the layout specified in the vertex shader.
         *
         * eg. GLSL:
         *
         * layout(binding = 3) uniform sampler2D mainTexture;
         * layout(binding = 1, std140) uniform MainBlock
         * {
         *      vec3 data;
         * };
         *
         * @param decompiler If the implementation does not support SPIRV natively this decompiler is used to decompile the spirv to the target language.
         */
        virtual std::unique_ptr<ShaderProgram> createShaderProgram(const SPIRVDecompiler &decompiler,
                                                                   const ShaderProgramDesc &desc) = 0;

        virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDesc &desc,
                                                                     ShaderProgram &shader) = 0;

        virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const uint8_t *cacheData, size_t size) = 0;

        virtual std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDesc &desc) = 0;

        virtual std::unique_ptr<RaytracePipeline> createRaytracePipeline(const RaytracePipelineDesc &desc) = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc &desc) = 0;

        virtual std::unique_ptr<TextureBuffer> createTextureBuffer(const TextureBufferDesc &desc) = 0;

        virtual std::unique_ptr<VertexBuffer> createVertexBuffer(const VertexBufferDesc &desc) = 0;

        virtual std::unique_ptr<ShaderBuffer> createShaderBuffer(const ShaderBufferDesc &desc) = 0;

        /**
         * eg. GLSL:
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *  layout (location = 5) in ivec4 boneIds;
         *  layout (location = 6) in vec4 boneWeights;
         *
         * @param mesh
         * @return
         */
        virtual std::unique_ptr<VertexBuffer> createVertexBuffer(const Mesh &mesh,
                                                                 RenderBufferType bufferType = DEVICE_LOCAL) {
            const std::vector<VertexAttribute> layout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::SIGNED_INT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
            };

            VertexBufferDesc desc = {
                    .primitive = mesh.primitive,
                    .vertexLayout = layout,
                    .numberOfVertices = mesh.vertices.size(),
                    .numberOfIndices = mesh.indices.size()
            };

            if (bufferType == DEVICE_LOCAL || bufferType == LAZILY_ALLOCATED) {
                // Use staging buffer for buffer allocation
                auto stagingBuffer = createVertexBuffer(desc);
                stagingBuffer->upload(mesh);
                desc.bufferType = bufferType;
                auto ret = createVertexBuffer(desc);
                ret->copy(*stagingBuffer);
                return ret;
            } else {
                desc.bufferType = bufferType;
                auto ret = createVertexBuffer(desc);
                ret->upload(mesh);
                return ret;
            }
        }

        /**
         * eg. GLSL:
         *  layout (location = 0) in vec3 position;
         *  layout (location = 1) in vec3 normal;
         *  layout (location = 2) in vec2 uv;
         *  layout (location = 3) in vec3 tangent;
         *  layout (location = 4) in vec3 bitangent;
         *  layout (location = 5) in ivec4 boneIds;
         *  layout (location = 6) in vec4 boneWeights;
         *  -- Instance Start --
         *  layout (location = 7) in vec4 instanceRow0;
         *  layout (location = 8) in vec4 instanceRow1;
         *  layout (location = 9) in vec4 instanceRow2;
         *  layout (location = 10) in vec4 instanceRow3;
         *
         * @param mesh
         * @param offsets
         * @return
         */
        virtual std::unique_ptr<VertexBuffer> createInstancedVertexBuffer(const Mesh &mesh,
                                                                          const std::vector<Mat4f> &offsets,
                                                                          RenderBufferType bufferType = DEVICE_LOCAL) {
            const std::vector<VertexAttribute> layout = {
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR2, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR3, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::SIGNED_INT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
            };

            const std::vector<VertexAttribute> instanceLayout = {
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT),
                    VertexAttribute(VertexAttribute::VECTOR4, VertexAttribute::FLOAT)
            };

            VertexBufferDesc desc = {
                    .primitive = mesh.primitive,
                    .vertexLayout = layout,
                    .instanceLayout = instanceLayout,
                    .numberOfVertices = mesh.vertices.size(),
                    .numberOfInstances = offsets.size(),
                    .numberOfIndices = mesh.indices.size(),
            };
            if (bufferType == DEVICE_LOCAL || bufferType == LAZILY_ALLOCATED) {
                // Use staging buffer for buffer allocation
                auto stagingBuffer = createVertexBuffer(desc);
                stagingBuffer->upload(mesh, offsets);
                desc.bufferType = bufferType;
                auto ret = createVertexBuffer(desc);
                ret->copy(*stagingBuffer);
                return ret;
            } else {
                auto ret = createVertexBuffer(desc);
                ret->upload(mesh, offsets);
                return ret;
            }

            if (bufferType == DEVICE_LOCAL || bufferType == LAZILY_ALLOCATED) {
                // Use staging buffer for buffer allocation
                auto stagingBuffer = createVertexBuffer(desc);
                stagingBuffer->upload(mesh, offsets);
                desc.bufferType = bufferType;
                auto ret = createVertexBuffer(desc);
                ret->copy(*stagingBuffer);
                return ret;
            } else {
                auto ret = createVertexBuffer(desc);
                ret->upload(mesh, offsets);
                return ret;
            }
        }
    };
}

#endif //XENGINE_RENDERDEVICE_HPP
