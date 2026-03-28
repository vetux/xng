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

#ifndef XENGINE_UPLOADCONTEXT_HPP
#define XENGINE_UPLOADCONTEXT_HPP

#include "xng/rendergraph/textureproperties.hpp"
#include "xng/rendergraph/resourceid.hpp"

#include "xng/rendergraph/resource/texture.hpp"
#include "xng/rendergraph/resource/indexbuffer.hpp"
#include "xng/rendergraph/resource/storagebuffer.hpp"
#include "xng/rendergraph/resource/vertexbuffer.hpp"

#include "xng/rendergraph/shader/shader.hpp"

#include "xng/math/vector2.hpp"

namespace xng::rendergraph {
    class TransferContext {
    public:
        virtual ~TransferContext() = default;

        /**
         * Upload data to a buffer.
         *
         * This is a convenience method.
         *
         * On Vulkan this is implemented with internal staging buffers.
         *
         * @param target
         * @param buffer The buffer holding the data
         * @param bufferSize The size of the buffer
         * @param targetOffset The offset into target to start writing to
         */
        virtual void uploadBuffer(const Resource<Buffer> &target,
                                  const uint8_t *buffer,
                                  size_t bufferSize,
                                  size_t targetOffset) = 0;

        /**
         * Upload data to a texture.
         *
         * This is a convenience method.
         *
         * The buffer is uploaded top to bottom, meaning the first row in the buffer corresponds to the top row.
         *
         * On Vulkan this is implemented with internal staging buffers.
         *
         * On OpenGL internally textures are still stored bottom to top,
         * but all visible readback is performed with the origin at top left.
         * (Shader compiler inverts uv.v accesses automatically, Upload and Copy from/to buffer inverts rows and coordinates internally)
         *
         * @param texture The texture to upload into
         * @param target The target subresource to upload into
         * @param buffer The buffer holding the pixel data
         * @param bufferSize The size of the buffer
         * @param bufferFormat The format of the pixel data in the buffer. Must be R/RG/RGB or RGBA
         * @param offset The offset (Origin top-left) into the texture to upload into
         * @param size The width / height of the pixel data in the buffer. Must be smaller or equal to the texture size.
         */
        virtual void uploadTexture(const Resource<Texture> &texture,
                                   Texture::SubResource target,
                                   const uint8_t *buffer,
                                   size_t bufferSize,
                                   ColorFormat bufferFormat,
                                   const Vec2i &offset,
                                   const Vec2i &size) = 0;

        /**
         * Copy data from one buffer to another.
         *
         * @param target The buffer to copy into
         * @param source The buffer to copy from
         * @param targetOffset The offset into target to write to
         * @param sourceOffset The offset into source to read from
         * @param count The number of bytes to copy
         */
        virtual void copyBuffer(const Resource<Buffer> &target,
                                const Resource<Buffer> &source,
                                size_t targetOffset,
                                size_t sourceOffset,
                                size_t count) = 0;

        void copyVertexBuffer(const Resource<VertexBuffer> &target,
                              const Resource<VertexBuffer> &source,
                              const size_t targetOffset,
                              const size_t sourceOffset,
                              const size_t count) {
            copyBuffer(target, source, targetOffset, sourceOffset, count);
        }

        void copyIndexBuffer(const Resource<IndexBuffer> &target,
                             const Resource<IndexBuffer> &source,
                             const size_t targetOffset,
                             const size_t sourceOffset,
                             const size_t count) {
            copyBuffer(target, source, targetOffset, sourceOffset, count);
        }

        void copyStorageBuffer(const Resource<StorageBuffer> &target,
                               const Resource<StorageBuffer> &source,
                               const size_t targetOffset,
                               const size_t sourceOffset,
                               const size_t count) {
            copyBuffer(target, source, targetOffset, sourceOffset, count);
        }

        /**
         * Copy a texel region of a mip level to a mip level of another texture.
         *
         * The offsets specify the x/y top left coordinates of the region to copy,
         * and z is the index of the layer for array textures.
         *
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
        virtual void copyTexture(const Resource<Texture> &target,
                                 const Resource<Texture> &source,
                                 const Vec3i &srcOffset,
                                 const Vec3i &dstOffset,
                                 const Vec3i &size,
                                 size_t srcMipMapLevel,
                                 size_t dstMipMapLevel) = 0;

        /**
         * Copy a buffer to a texture.
         *
         * The format of the pixel data in the buffer must be compatible with texture.
         *
         * The texture data is read from the buffer with the rows progressing vertically top to bottom
         * and each row progressing horizontally left to right.
         *
         * The textureOffset specifies the top left pixel position and extent of the region to write to.
         *
         * In GL the copy is performed by mapping the buffer and using an inverted copy of the pixel data
         * in the same way as uploadTexture because opengl expects the texture data in bottom-up format.
         *
         * @param texture The texture to copy into
         * @param buffer The buffer to copy from
         * @param textureSubResource The target sub resource to copy into.
         * @param bufferOffset The offset into the buffer at which to start reading.
         * @param textureOffset The region to write to.
         */
        virtual void copyBufferToTexture(const Resource<Texture> &texture,
                                         const Resource<Buffer> &buffer,
                                         Texture::SubResource textureSubResource,
                                         size_t bufferOffset,
                                         const Recti &textureOffset) = 0;

        /**
         * Copy a texture to a buffer.
         *
         * The texture data is stored in the buffer with the rows progressing vertically top to bottom
         * and each row progressing horizontally left to right.
         *
         * The textureOffset specifies the top left pixel position and extent of the region to read from.
         *
         * In GL the copy is performed by mapping the buffer after the copy and inverting the stored data before returning.
         *
         * @param buffer The buffer to copy into
         * @param texture The texture to copy from
         * @param textureSubResource The target sub resource to copy from
         * @param bufferOffset The offset into the buffer at which to start writing
         * @param textureOffset The region to read from.
         */
        virtual void copyTextureToBuffer(const Resource<Buffer> &buffer,
                                         const Resource<Texture> &texture,
                                         Texture::SubResource textureSubResource,
                                         size_t bufferOffset,
                                         const Recti &textureOffset) = 0;

        /**
         * Clear a texture.
         *
         * @param texture
         * @param target
         * @param clearValue
         */
        virtual void clearTexture(const Resource<Texture> &texture,
                                  const Texture::SubResource &target,
                                  const Texture::ClearValue &clearValue) = 0;

        /**
          * Automatically generate mip maps for the given texture.
          *
          * Mip Maps are generated based on the given Texture.mipMapLevels count.
          *
          * @param texture The texture for which to generate mip maps for.
          */
        virtual void generateMipMaps(const Resource<Texture> &texture) = 0;
    };
}

#endif //XENGINE_UPLOADCONTEXT_HPP
