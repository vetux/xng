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

#ifndef XENGINE_FRAMEGRAPHATTACHMENT_HPP
#define XENGINE_FRAMEGRAPHATTACHMENT_HPP


#include "xng/gpu/rendertargetattachment.hpp"

#include "xng/render/graph/framegraphresource.hpp"

namespace xng {
    struct FrameGraphAttachment {
        FrameGraphResource resource;
        RenderTargetAttachment::AttachmentType type{};
        size_t index{};
        CubeMapFace face{};
        size_t mipMapLevel{};

        static FrameGraphAttachment texture(FrameGraphResource textureBuffer, size_t mipMapLevel = 0) {
            return {textureBuffer, RenderTargetAttachment::ATTACHMENT_TEXTURE, {}, {}, mipMapLevel};
        }

        static FrameGraphAttachment cubemap(FrameGraphResource textureBuffer, CubeMapFace face, size_t mipMapLevel = 0) {
            return {textureBuffer, RenderTargetAttachment::ATTACHMENT_CUBEMAP, {}, face, mipMapLevel};
        }

        /**
         * Create a layered cube map texture attachment where shaders select which face of the cube map to write to by using eg gl_Layer in glsl
         *
         * @param textureBuffer
         * @return
         */
        static FrameGraphAttachment cubemapLayered(FrameGraphResource textureBuffer, size_t mipMapLevel = 0) {
            return {textureBuffer, RenderTargetAttachment::ATTACHMENT_CUBEMAP_LAYERED, {}, {}, mipMapLevel};
        }

        /**
         * Attach the texture 2d at the specified index in the textureArrayBuffer to the target.
         *
         * @param textureArrayBuffer
         * @param index
         * @param mipMapLevel
         * @return
         */
        static FrameGraphAttachment textureArray(FrameGraphResource textureArrayBuffer,
                                                 size_t index,
                                                 size_t mipMapLevel = 0) {
            return {textureArrayBuffer, RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY, index, {}, mipMapLevel};
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
        static FrameGraphAttachment textureArrayCubeMap(FrameGraphResource textureArrayBuffer,
                                                        size_t index,
                                                        CubeMapFace face,
                                                        size_t mipMapLevel = 0) {
            return {textureArrayBuffer, RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY_CUBEMAP, index, face,
                    mipMapLevel};
        }

        /**
         * Create a 2d or cube map texture attachment where shaders select which texture in the array to write to by using eg gl_Layer in glsl.
         * If the texture is a cube map array texture the shader can select the index and face to write to by setting gl_Layer to (index * 6) + face
         *
         * @param textureArrayBuffer
         */
        static FrameGraphAttachment textureArrayLayered(FrameGraphResource textureArrayBuffer,
                                                        size_t mipMapLevel = 0) {
            return {textureArrayBuffer, RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY_LAYERED, {}, {},
                    mipMapLevel};
        }
    };
}

#endif //XENGINE_FRAMEGRAPHATTACHMENT_HPP
