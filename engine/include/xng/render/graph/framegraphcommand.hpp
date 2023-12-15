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

#ifndef XENGINE_FRAMEGRAPHCOMMAND_HPP
#define XENGINE_FRAMEGRAPHCOMMAND_HPP

#include <vector>
#include <functional>
#include <cstring>

#include "xng/render/graph/framegraphresource.hpp"

#include "xng/gpu/drawcall.hpp"

namespace xng {
    struct FrameGraphCommand {
        enum Type {
            CREATE_RENDER_PIPELINE,
            CREATE_TEXTURE,
            CREATE_TEXTURE_ARRAY,
            CREATE_VERTEX_BUFFER,
            CREATE_INDEX_BUFFER,
            CREATE_VERTEX_ARRAY_OBJECT,
            CREATE_SHADER_UNIFORM_BUFFER,
            CREATE_SHADER_STORAGE_BUFFER,
            UPLOAD,
            COPY,
            BLIT_COLOR,
            BLIT_DEPTH,
            BLIT_STENCIL,
            CLEAR_TEXTURE_COLOR,
            CLEAR_TEXTURE_FLOAT,
            BEGIN_PASS,
            FINISH_PASS,
            RENDER_CLEAR,
            BIND_PIPELINE,
            BIND_VERTEX_ARRAY_OBJECT,
            BIND_VERTEX_ARRAY_OBJECT_BUFFERS,
            BIND_SHADER_RESOURCES,
            SET_VIEWPORT,
            DRAW_ARRAY,
            DRAW_INDEXED,
            DRAW_INSTANCED_ARRAY,
            DRAW_INSTANCED_INDEXED,
            DRAW_MULTI_ARRAY,
            DRAW_MULTI_INDEXED,
            DRAW_INDEXED_BASE_VERTEX,
            DRAW_INSTANCED_INDEXED_BASE_VERTEX,
            DRAW_MULTI_INDEXED_BASE_VERTEX,
        } type;

        struct UploadBuffer {
            size_t size;
            uint8_t *data;

            UploadBuffer() = default;

            UploadBuffer(size_t size, const uint8_t *v) : size(size),
                                                        data(new uint8_t[size]) {
                std::memcpy(data, v, size);
            }

            ~UploadBuffer() {
                delete[] data;
            }
        };

        struct UploadData {
            size_t index;
            size_t offset;
            std::function<UploadBuffer()> dataSource;
        };

        struct DrawCallData {
            std::vector<DrawCall> drawCalls;
            std::vector<size_t> baseVertices;
            size_t numberOfInstances;
        };

        struct ClearData {
            ColorRGBA color;
            float value;
        };

        struct ViewportData {
            Vec2i viewportOffset;
            Vec2i viewportSize;
        };

        struct BlitData {
            Vec2i sourceOffset;
            Vec2i targetOffset;
            Vec2i sourceRect;
            Vec2i targetRect;
            TextureFiltering filter;
            int sourceIndex;
            int targetIndex;
        };

        struct ShaderData {
            FrameGraphResource resource;
            std::map<ShaderStage, ShaderResource::AccessMode> accessModes;
        };

        struct Attachment {
            FrameGraphResource resource;
            RenderTargetAttachment::AttachmentType type{};
            size_t index{};
            CubeMapFace face{};
            size_t mipMapLevel{};

            static Attachment texture(FrameGraphResource textureBuffer, size_t mipMapLevel = 0) {
                return {textureBuffer, RenderTargetAttachment::ATTACHMENT_TEXTURE, {}, {}, mipMapLevel};
            }

            static Attachment cubemap(FrameGraphResource textureBuffer, CubeMapFace face, size_t mipMapLevel = 0) {
                return {textureBuffer, RenderTargetAttachment::ATTACHMENT_CUBEMAP, {}, face, mipMapLevel};
            }

            /**
             * Create a layered cube map texture attachment where shaders select which face of the cube map to write to by using eg gl_Layer in glsl
             *
             * @param textureBuffer
             * @return
             */
            static Attachment cubemapLayered(FrameGraphResource textureBuffer, size_t mipMapLevel = 0) {
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
            static Attachment textureArray(FrameGraphResource textureArrayBuffer,
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
            static Attachment textureArrayCubeMap(FrameGraphResource textureArrayBuffer,
                                                              size_t index,
                                                              CubeMapFace face,
                                                              size_t mipMapLevel = 0) {
                return {textureArrayBuffer, RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY_CUBEMAP, index, face, mipMapLevel};
            }

            /**
             * Create a 2d or cube map texture attachment where shaders select which texture in the array to write to by using eg gl_Layer in glsl.
             * If the texture is a cube map array texture the shader can select the index and face to write to by setting gl_Layer to (index * 6) + face
             *
             * @param textureArrayBuffer
             */
            static Attachment textureArrayLayered(FrameGraphResource textureArrayBuffer,
                                                              size_t mipMapLevel = 0) {
                return {textureArrayBuffer, RenderTargetAttachment::ATTACHMENT_TEXTUREARRAY_LAYERED, {}, {}, mipMapLevel};
            }
        };

        struct BeginPassData {
            std::vector<Attachment> colorAttachments;
            Attachment depthAttachment;
        };

        std::variant<UploadData,
                DrawCallData,
                ClearData,
                ViewportData,
                BlitData,
                std::vector<ShaderData>,
                BeginPassData,
                RenderTargetDesc,
                RenderPipelineDesc,
                TextureBufferDesc,
                TextureArrayBufferDesc,
                VertexBufferDesc,
                IndexBufferDesc,
                VertexArrayObjectDesc,
                ShaderUniformBufferDesc,
                ShaderStorageBufferDesc> data;

        std::vector<FrameGraphResource> resources;
    };
}
#endif //XENGINE_FRAMEGRAPHCOMMAND_HPP
