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

#ifndef XENGINE_UPLOADCONTEXT_HPP
#define XENGINE_UPLOADCONTEXT_HPP

#include "xng/rendergraph/textureproperties.hpp"
#include "xng/rendergraph/resourceid.hpp"

#include "xng/rendergraph/resource/texture.hpp"
#include "xng/rendergraph/resource/buffer.hpp"

#include "xng/rendergraph/shader/shader.hpp"

#include "xng/math/vector3.hpp"
#include "xng/math/rectangle.hpp"

namespace xng::rg {
    class TransferContext {
    public:
        /**
         * The offsets specify the x/y top left coordinates of the region to copy.
         *
         * The size specifies the width/height of the region to copy.
         */
        struct TextureCopyRegion {
            Texture::SubResource src;
            Texture::SubResource dst;
            Vec2i srcOffset;
            Vec2i dstOffset;
            Vec2i size;
        };

        virtual ~TransferContext() = default;

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

        /**
         * Copy texel regions from one texture to another.
         *
         * @param target
         * @param source
         * @param regions
         */
        virtual void copyTexture(const Resource<Texture> &target,
                                 const Resource<Texture> &source,
                                 const std::vector<TextureCopyRegion> &regions) = 0;

        /**
         * Copy a buffer to a texture.
         *
         * The format of the pixel data in the buffer must be compatible with the texture format.
         * The implementation will perform conversion between color formats.
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
         * @param bufferFormat The format of the pixel data in the buffer.
         */
        virtual void copyBufferToTexture(const Resource<Texture> &texture,
                                         const Resource<Buffer> &buffer,
                                         Texture::SubResource textureSubResource,
                                         size_t bufferOffset,
                                         const Recti &textureOffset,
                                         ColorFormat bufferFormat) = 0;

        /**
         * Copy a texture to a buffer.
         *
         * The implementation performs conversion between texture format / buffer format.
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
         * @param bufferFormat The format that the texture pixel data should be stored as in the buffer.
         */
        virtual void copyTextureToBuffer(const Resource<Buffer> &buffer,
                                         const Resource<Texture> &texture,
                                         Texture::SubResource textureSubResource,
                                         size_t bufferOffset,
                                         const Recti &textureOffset,
                                         ColorFormat bufferFormat) = 0;

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
         * Blit a sub resource rect of one texture into a sub resource rect of another texture.
         * Can be used for mipmap generation.
         *
         * The blit rects are specified with:
         * 0,0 coordinates representing the top left corner of the texture and x increasing to the right and y increasing downwards.
         *
         * @param src The texture to blit from
         * @param dst The texture to blit into
         * @param srcTarget The sub resource of the source texture to blit from
         * @param dstTarget The sub resource of the target texture to blit into
         * @param srcRect The source rect to blit
         * @param dstRect The target rect to blit into
         * @param filtering The filtering to apply if the blit rects are not equal size.
         */
        virtual void blitTexture(const Resource<Texture> &src,
                                 const Resource<Texture> &dst,
                                 const Texture::SubResource &srcTarget,
                                 const Texture::SubResource &dstTarget,
                                 const Recti& srcRect,
                                 const Recti& dstRect,
                                 const TextureFiltering &filtering) = 0;

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
