/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_RENDERTARGETATTACHMENT_HPP
#define XENGINE_RENDERTARGETATTACHMENT_HPP

#include <variant>

#include "xng/gpu/textureproperties.hpp"

namespace xng {
    class TextureBuffer;

    class TextureArrayBuffer;

    struct RenderTargetAttachment {
        enum AttachmentType : int {
            ATTACHMENT_TEXTURE = 0,
            ATTACHMENT_CUBEMAP,
            ATTACHMENT_CUBEMAP_LAYERED,
            ATTACHMENT_TEXTUREARRAY,
            ATTACHMENT_TEXTUREARRAY_CUBEMAP,
            ATTACHMENT_TEXTUREARRAY_LAYERED,
        };

        RenderTargetAttachment() = default;

        RenderTargetAttachment(AttachmentType type,
                               TextureBuffer &textureBuffer,
                               size_t mipMapLevel)
                : type(type),
                  textureBuffer(&textureBuffer),
                  mipMapLevel(mipMapLevel) {}

        RenderTargetAttachment(AttachmentType type,
                               TextureBuffer &textureBuffer,
                               CubeMapFace face,
                               size_t mipMapLevel)
                : type(type),
                  textureBuffer(&textureBuffer),
                  face(face),
                  mipMapLevel(mipMapLevel)  {}

        RenderTargetAttachment(AttachmentType type,
                               TextureArrayBuffer &textureArrayBuffer,
                               size_t mipMapLevel)
                : type(type),
                  textureArrayBuffer(&textureArrayBuffer),
                  mipMapLevel(mipMapLevel)  {}

        RenderTargetAttachment(AttachmentType type,
                               TextureArrayBuffer &textureArrayBuffer,
                               size_t index,
                               size_t mipMapLevel)
                : type(type),
                  textureArrayBuffer(&textureArrayBuffer),
                  index(index),
                  mipMapLevel(mipMapLevel)  {}

        RenderTargetAttachment(AttachmentType type,
                               TextureArrayBuffer &textureArrayBuffer,
                               size_t index,
                               CubeMapFace face,
                               size_t mipMapLevel)
                : type(type),
                  textureArrayBuffer(&textureArrayBuffer),
                  index(index),
                  face(face),
                  mipMapLevel(mipMapLevel)  {}

        static RenderTargetAttachment texture(TextureBuffer &textureBuffer, size_t mipMapLevel = 0) {
            return {ATTACHMENT_TEXTURE, textureBuffer, mipMapLevel};
        }

        static RenderTargetAttachment cubemap(TextureBuffer &textureBuffer, CubeMapFace face, size_t mipMapLevel = 0) {
            return {ATTACHMENT_CUBEMAP, textureBuffer, face, mipMapLevel};
        }

        /**
         * Create a layered cube map texture attachment where shaders select which face of the cube map to write to by using eg gl_Layer in glsl
         *
         * @param textureBuffer
         * @return
         */
        static RenderTargetAttachment cubemapLayered(TextureBuffer &textureBuffer, size_t mipMapLevel = 0) {
            return {ATTACHMENT_CUBEMAP_LAYERED, textureBuffer, mipMapLevel};
        }

        /**
         * Attach the texture 2d at the specified index in the textureArrayBuffer to the target.
         *
         * @param textureArrayBuffer
         * @param index
         * @param mipMapLevel
         * @return
         */
        static RenderTargetAttachment textureArray(TextureArrayBuffer &textureArrayBuffer,
                                                   size_t index,
                                                   size_t mipMapLevel = 0) {
            return {ATTACHMENT_TEXTUREARRAY, textureArrayBuffer, index, mipMapLevel};
        }

        /**
         * Attach the face of the cube map at the specified index in the textureArrayBuffer to the target.
         *
         * @param textureArrayBuffer
         * @param index
         * @param face
         * @param mipMapLevel
         * @return
         */
        static RenderTargetAttachment textureArrayCubeMap(TextureArrayBuffer &textureArrayBuffer,
                                                          size_t index,
                                                          CubeMapFace face,
                                                          size_t mipMapLevel = 0) {
            return {ATTACHMENT_TEXTUREARRAY_CUBEMAP, textureArrayBuffer, index, face, mipMapLevel};
        }

        /**
         * Create a 2d or cube map texture attachment where shaders select which texture in the array to write to by using eg gl_Layer in glsl.
         * If the texture is a cube map array texture the shader can select the index and face to write to by setting gl_Layer to (index * 6) + face
         *
         * @param textureArrayBuffer
         */
        static RenderTargetAttachment textureArrayLayered(TextureArrayBuffer &textureArrayBuffer,
                                                          size_t mipMapLevel = 0) {
            return {ATTACHMENT_TEXTUREARRAY_LAYERED, textureArrayBuffer, mipMapLevel};
        }

        AttachmentType type{};
        TextureBuffer *textureBuffer{};
        TextureArrayBuffer *textureArrayBuffer{};
        size_t index{};
        CubeMapFace face{};
        size_t mipMapLevel{};
    };
}

#endif //XENGINE_RENDERTARGETATTACHMENT_HPP
