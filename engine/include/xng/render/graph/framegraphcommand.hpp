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
#include "xng/render/graph/framegraphattachment.hpp"

#include "xng/gpu/drawcall.hpp"

namespace xng {
    struct FrameGraphCommand {
        enum Type : int {
            CREATE_RENDER_PIPELINE = 0,
            CREATE_TEXTURE,
            CREATE_TEXTURE_ARRAY,
            CREATE_VERTEX_BUFFER,
            CREATE_INDEX_BUFFER,
            CREATE_SHADER_UNIFORM_BUFFER,
            CREATE_SHADER_STORAGE_BUFFER,
            UPLOAD,
            COPY,
            BLIT_COLOR,
            BLIT_DEPTH,
            BLIT_STENCIL,
            BEGIN_PASS,
            FINISH_PASS,
            CLEAR_COLOR,
            CLEAR_DEPTH,
            BIND_PIPELINE,
            BIND_VERTEX_BUFFERS,
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
            ColorFormat colorFormat;
            CubeMapFace face;
            std::function<UploadBuffer()> dataSource;
        };

        struct CopyData {
            size_t readOffset;
            size_t writeOffset;
            size_t count;
        };

        struct DrawCallData {
            std::vector<DrawCall> drawCalls;
            std::vector<size_t> baseVertices;
            size_t numberOfInstances;
        };

        struct ClearData {
            ColorRGBA color;
            float depth;
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

        struct BindVertexData {
            VertexLayout vertexLayout{};
            VertexLayout instanceArrayLayout{};
        };

        struct ShaderData {
            FrameGraphResource resource;
            std::map<ShaderStage, ShaderResource::AccessMode> accessModes;
        };

        struct BeginPassData {
            std::vector<FrameGraphAttachment> colorAttachments;
            FrameGraphAttachment depthAttachment;
        };

        std::variant<UploadData,
                CopyData,
                DrawCallData,
                ClearData,
                ViewportData,
                BlitData,
                BindVertexData,
                std::vector<ShaderData>,
                BeginPassData,
                RenderPipelineDesc,
                TextureBufferDesc,
                TextureArrayBufferDesc,
                VertexBufferDesc,
                IndexBufferDesc,
                ShaderUniformBufferDesc,
                ShaderStorageBufferDesc> data;

        std::vector<FrameGraphResource> resources;
    };
}
#endif //XENGINE_FRAMEGRAPHCOMMAND_HPP
