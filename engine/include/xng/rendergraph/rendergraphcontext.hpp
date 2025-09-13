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

#ifndef XENGINE_RENDERGRAPHCONTEXT_HPP
#define XENGINE_RENDERGRAPHCONTEXT_HPP

#include <cstdint>

#include "xng/rendergraph/drawcall.hpp"
#include "xng/rendergraph/rendergraphresource.hpp"
#include "xng/rendergraph/rendergraphtextureproperties.hpp"
#include "xng/rendergraph/shader/shaderstage.hpp"
#include "xng/rendergraph/shader/shaderliteral.hpp"

#include "xng/render/scene/image.hpp"

namespace xng {
    class RenderGraphContext {
    public:
        virtual ~RenderGraphContext() = default;

        /**
         * Upload data to a buffer.
         * Subsequent draw() invocations that use the specified buffer are guaranteed to receive the uploaded data.
         * The upload might be performed asynchronously.
         *
         * @param buffer
         * @param ptr
         * @param size
         */
        virtual void uploadBuffer(RenderGraphResource buffer, const uint8_t *ptr, size_t size) = 0;

        /**
         * Upload data to a texture.
         * Subsequent draw() invocations that use the specified texture are guaranteed to receive the uploaded data.
         * The upload might be performed asynchronously.
         *
         * @param texture
         * @param ptr
         * @param size
         * @param format
         * @param index
         * @param mipMapLevel
         * @param face
         */
        virtual void uploadTexture(RenderGraphResource texture,
                                   const uint8_t *ptr,
                                   size_t size,
                                   ColorFormat format,
                                   size_t index = 0,
                                   size_t mipMapLevel = 0,
                                   CubeMapFace face = POSITIVE_X) = 0;

        /**
         * Bind the given pipeline.
         *
         * Resets previous binding state, so this should be called before binding the desired buffers.
         *
         * @param pipeline
         */
        virtual void bindPipeline(RenderGraphResource pipeline) = 0;

        virtual void bindVertexBuffer(RenderGraphResource buffer) = 0;

        virtual void bindIndexBuffer(RenderGraphResource buffer) = 0;

        /**
         * Bind a texture which can be written to by Fragment shaders.
         *
         * To render to the screen, the texture returned by RenderGraphBuilder.getScreenTexture must be explicitly bound.
         *
         * @param binding
         * @param texture
         * @param index
         * @param mipMapLevel
         * @param face
         */
        virtual void bindRenderTarget(size_t binding,
                                      RenderGraphResource texture,
                                      size_t index = 0,
                                      size_t mipMapLevel = 0,
                                      CubeMapFace face = POSITIVE_X) = 0;

        virtual void bindTextures(const std::unordered_map<std::string, RenderGraphResource> &textures) = 0;

        virtual void bindShaderBuffers(const std::unordered_map<std::string, RenderGraphResource> &buffers) = 0;

        /**
         * Set the shader parameters for the next draw call. (Implemented as Push Constants)
         *
         * Shader parameters are values that change frequently (Per Draw) and have a size limit.
         *
         * Shader Parameters are set per draw call and must be set again for subsequent draw calls.
         *
         * @param parameters
         */
        virtual void setShaderParameters(const std::unordered_map<std::string, ShaderLiteral> &parameters) = 0;

        /**
         * Execute the draw call/s with the bound shaders and geometry data.
         *
         * @param calls
         */
        virtual void draw(const std::vector<DrawCall> &calls) = 0;

        /**
         * Download the data from the gpu side shader buffer.
         *
         * Should be avoided as it forces the runtime to execute all previously recorded operations that affect the specified buffer.
         *
         * @param buffer
         * @return
         */
        virtual std::vector<uint8_t> downloadShaderBuffer(RenderGraphResource buffer) = 0;

        /**
         * Download the data from the gpu side texture.
         *
         * Should be avoided as it forces the runtime to execute all previously recorded operations that affect the specified texture.
         *
         * @param texture
         * @param index
         * @param mipMapLevel
         * @param face
         * @return
         */
        virtual Image<ColorRGBA> downloadTexture(RenderGraphResource texture,
                                                 size_t index = 0,
                                                 size_t mipMapLevel = 0,
                                                 CubeMapFace face = POSITIVE_X) = 0;

        /**
         * Return the internal compiled source code of the supplied pipeline.
         *
         * For debugging purposes only.
         *
         * @param pipeline
         * @return
         */
        virtual std::unordered_map<ShaderStage::Type, std::string> getShaderSource(RenderGraphResource pipeline) = 0;
    };
}

#endif //XENGINE_RENDERGRAPHCONTEXT_HPP
