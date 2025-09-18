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
#include "xng/rendergraph/rendergraphattachment.hpp"
#include "xng/rendergraph/shader/shader.hpp"
#include "xng/rendergraph/shader/shaderliteral.hpp"

#include "xng/math/vector2.hpp"

#include "xng/render/scene/image.hpp"

namespace xng {
    class RenderGraphContext {
    public:
        virtual ~RenderGraphContext() = default;

        /**
         * Upload data to a buffer.
         *
         * Subsequent access to the target buffer is guaranteed to receive the uploaded data.
         *
         * @param target
         * @param buffer The buffer holding the data
         * @param bufferSize The size of the buffer
         * @param targetOffset The offset into target to start writing to
         */
        virtual void uploadBuffer(RenderGraphResource target,
                                  const uint8_t *buffer,
                                  size_t bufferSize,
                                  size_t targetOffset) = 0;

        /**
         * Upload data to a texture.
         *
         * Subsequent access to the texture is guaranteed to receive the uploaded data.
         *
         * @param texture
         * @param buffer The buffer holding the pixel data
         * @param bufferSize The size of the buffer
         * @param bufferFormat The format of the pixel data in the buffer, must be one of the base formats in ColorFormat
         * @param index The index of the texture to upload into if the passed texture is an array texture
         * @param face The face of the texture to upload into if texture is a cube map texture
         * @param mipMapLevel
         */
        virtual void uploadTexture(RenderGraphResource texture,
                                   const uint8_t *buffer,
                                   size_t bufferSize,
                                   ColorFormat bufferFormat,
                                   size_t index,
                                   CubeMapFace face,
                                   size_t mipMapLevel) = 0;

        void uploadTexture(const RenderGraphResource texture,
                           const uint8_t *buffer,
                           const size_t bufferSize,
                           const ColorFormat format) {
            uploadTexture(texture, buffer, bufferSize, format, 0, {}, 0);
        }

        void uploadTexture(const RenderGraphResource texture,
                           const uint8_t *buffer,
                           const size_t bufferSize,
                           const ColorFormat format,
                           const size_t index) {
            uploadTexture(texture, buffer, bufferSize, format, index, {}, 0);
        }

        virtual void clearTextureColor(RenderGraphResource texture,
                                       const ColorRGBA &clearColor,
                                       size_t index,
                                       CubeMapFace face,
                                       size_t mipMapLevel) = 0;

        void clearTextureColor(const RenderGraphResource texture, const ColorRGBA &clearColor) {
            clearTextureColor(texture, clearColor, 0, {}, 0);
        }

        virtual void clearTextureColor(RenderGraphResource texture,
                                       const Vec4f &clearColor,
                                       size_t index,
                                       CubeMapFace face,
                                       size_t mipMapLevel) = 0;

        void clearTextureColor(const RenderGraphResource texture, const Vec4f &clearColor) {
            clearTextureColor(texture, clearColor, 0, {}, 0);
        }


        virtual void clearTextureColor(RenderGraphResource texture,
                                       const Vec4i &clearColor,
                                       size_t index,
                                       CubeMapFace face,
                                       size_t mipMapLevel) = 0;

        void clearTextureColor(const RenderGraphResource texture, const Vec4i &clearColor) {
            clearTextureColor(texture, clearColor, 0, {}, 0);
        }

        virtual void clearTextureColor(RenderGraphResource texture,
                                       const Vec4u &clearColor,
                                       size_t index,
                                       CubeMapFace face,
                                       size_t mipMapLevel) = 0;

        void clearTextureColor(const RenderGraphResource texture, const Vec4u &clearColor) {
            clearTextureColor(texture, clearColor, 0, {}, 0);
        }

        virtual void clearTextureDepthStencil(RenderGraphResource texture,
                                              float clearDepth,
                                              unsigned int clearStencil,
                                              size_t index,
                                              CubeMapFace face,
                                              size_t mipMapLevel) = 0;

        void clearTextureDepthStencil(const RenderGraphResource texture,
                                      const float clearDepth,
                                      const unsigned int clearStencil) {
            clearTextureDepthStencil(texture, clearDepth, clearStencil, 0, {}, 0);
        }

        /**
         * Copy data from one buffer to another.
         *
         * Subsequent access to the buffer is guaranteed to receive the copied data.
         *
         * @param target The buffer to copy into
         * @param source The buffer to copy from
         * @param targetOffset The offset into target to write to
         * @param sourceOffset The offset into source to read from
         * @param count The number of bytes to copy
         */
        virtual void copyBuffer(RenderGraphResource target,
                                RenderGraphResource source,
                                size_t targetOffset,
                                size_t sourceOffset,
                                size_t count) = 0;

        /**
         * Copy one texture into another. The textures must have compatible formats / sizes.
         *
         * Subsequent access to the texture is guaranteed to receive the copied data.
         *
         * @param target The texture to copy into
         * @param source The texture to copy from
         */
        virtual void copyTexture(RenderGraphResource target, RenderGraphResource source) = 0;

        /**
         * Copy a subregion of a texture to another texture.
         *
         * Subsequent access to the texture is guaranteed to receive the copied data.
         *
         * The offsets specify the x/y top left coordinates of the region to copy, and z is the index of the layer for array textures.
         * The size specifies the width/height of the region to copy while z the number of layers to copy for array textures.
         *
         * @param target
         * @param source
         * @param srcOffset
         * @param dstOffset
         * @param size
         * @param srcMipMapLevel
         * @param dstMipMapLevel
         */
        virtual void copyTexture(RenderGraphResource target,
                                 RenderGraphResource source,
                                 Vec3i srcOffset,
                                 Vec3i dstOffset,
                                 Vec3i size,
                                 size_t srcMipMapLevel,
                                 size_t dstMipMapLevel) = 0;

        /**
         * Begin a render pass.
         *
         * Bindings must be set inside a render pass.
         *
         * Copy/upload operations can be performed inside a render pass.
         *
         * Download operations must be performed outside a render pass.
         *
         * @param colorAttachments
         * @param depthAttachment
         * @param stencilAttachment
         */
        virtual void beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
                                     const RenderGraphAttachment &depthAttachment,
                                     const RenderGraphAttachment &stencilAttachment) = 0;

        virtual void beginRenderPass(const std::vector<RenderGraphAttachment> &colorAttachments,
                                     const RenderGraphAttachment &depthStencilAttachment) = 0;

        virtual void bindPipeline(RenderGraphResource pipeline) = 0;

        /**
         * Bind the specified vertex buffer.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param buffer
         */
        virtual void bindVertexBuffer(RenderGraphResource buffer) = 0;

        /**
         * Bind the specified index buffer.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param buffer
         */
        virtual void bindIndexBuffer(RenderGraphResource buffer) = 0;

        /**
         * Bind the specified textures.
         *
         * Every sub vector represents a texture array in Shader.textureArrays
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param textureArrays
         */
        virtual void bindTextures(const std::unordered_map<std::string,
            std::vector<RenderGraphResource> > &textureArrays) = 0;

        /**
         * Bind the specified buffers.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param buffers
         */
        virtual void bindShaderBuffers(const std::unordered_map<std::string, RenderGraphResource> &buffers) = 0;

        /**
         * Set the shader parameters for the next draw call. (Implemented as Push Constants)
         *
         * Shader parameters are values that change frequently (Per Draw) and have a size limit.
         *
         * Shader Parameters are set per draw call and must be set again for subsequent draw calls.
         *
         * Can only be called after a pipeline has been bound.
         *
         * @param parameters
         */
        virtual void setShaderParameters(const std::unordered_map<std::string, ShaderLiteral> &parameters) = 0;

        virtual void clearColorAttachment(size_t binding, ColorRGBA clearColor) = 0;

        virtual void clearColorAttachment(size_t binding, const Vec4i &clearColor) = 0;

        virtual void clearColorAttachment(size_t binding, const Vec4u &clearColor) = 0;

        virtual void clearDepthStencilAttachment(float clearDepth, unsigned int clearStencil) = 0;

        virtual void clearDepthAttachment(float clearDepth) = 0;

        virtual void clearStencilAttachment(unsigned int clearStencil) = 0;

        virtual void setViewport(Vec2i viewportOffset, Vec2i viewportSize) = 0;

        virtual void drawArray(const DrawCall &drawCall) = 0;

        virtual void drawIndexed(const DrawCall &drawCall, size_t indexOffset) = 0;

        //TODO: Add instancing support

        virtual void endRenderPass() = 0;

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
        virtual std::unordered_map<Shader::Stage, std::string> getShaderSource(RenderGraphResource pipeline) = 0;
    };
}

#endif //XENGINE_RENDERGRAPHCONTEXT_HPP
