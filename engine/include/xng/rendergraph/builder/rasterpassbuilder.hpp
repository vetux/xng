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

#ifndef XENGINE_RASTERPASSBUILDER_HPP
#define XENGINE_RASTERPASSBUILDER_HPP

#include <functional>

#include "xng/rendergraph/pass.hpp"

namespace xng::rendergraph {
    class RasterPassBuilder {
    public:
        RasterPassBuilder(std::string name, std::function<void(RasterPass &&)> buildCallback)
            : buildCallback(std::move(buildCallback)) {
            pass.name = std::move(name);
        }

        RasterPassBuilder &read(const Resource<VertexBuffer> &buffer,
                                const size_t offset = 0,
                                const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::VertexRead, offset, size);
            const auto entry = RasterResourceAccess<BufferAccess>::Entry(Shader::VERTEX, access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        RasterPassBuilder &read(const Resource<IndexBuffer> &buffer,
                                const size_t offset = 0,
                                const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::IndexRead, offset, size);
            const auto entry = RasterResourceAccess<BufferAccess>::Entry(Shader::VERTEX, access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        RasterPassBuilder &read(const Resource<StorageBuffer> &buffer,
                                std::unordered_set<Shader::Stage> stages,
                                const size_t offset = 0,
                                const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::StorageRead, offset, size);
            const auto entry = RasterResourceAccess<BufferAccess>::Entry(std::move(stages), access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        RasterPassBuilder &write(const Resource<StorageBuffer> &buffer,
                                 std::unordered_set<Shader::Stage> stages,
                                 const size_t offset = 0,
                                 const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::StorageWrite, offset, size);
            const auto entry = RasterResourceAccess<BufferAccess>::Entry(std::move(stages), access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        RasterPassBuilder &sample(const Resource<Texture> &texture,
                                  std::unordered_set<Shader::Stage> stages,
                                  const TextureBinding::Range range = {},
                                  const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureSampledRead,
                                              range,
                                              aspect);
            const auto entry = RasterResourceAccess<TextureAccess>::Entry(std::move(stages), access);
            pass.textureUsages[texture].entries.emplace_back(entry);
            return *this;
        }

        RasterPassBuilder &attachColor(Attachment attachment) {
            pass.colorAttachments.emplace_back(std::move(attachment));
            return *this;
        }

        RasterPassBuilder &attachDepthStencil(Attachment attachment) {
            pass.depthStencilAttachment = attachment;
            return *this;
        }

        RasterPassBuilder &attachDepth(const Attachment &attachment) {
            if (!pass.depthStencilAttachment.has_value()
                || !std::holds_alternative<
                    RasterPass::DepthStencilAttachment>(pass.depthStencilAttachment.value())) {
                pass.depthStencilAttachment = RasterPass::DepthStencilAttachment{{}, {}};
            }
            std::get<RasterPass::DepthStencilAttachment>(pass.depthStencilAttachment.value())
                    .depthAttachment = attachment;
            return *this;
        }

        RasterPassBuilder &attachStencil(const Attachment &attachment) {
            if (!pass.depthStencilAttachment.has_value()
                || !std::holds_alternative<
                    RasterPass::DepthStencilAttachment>(pass.depthStencilAttachment.value())) {
                pass.depthStencilAttachment = RasterPass::DepthStencilAttachment{{}, {}};
            }
            std::get<RasterPass::DepthStencilAttachment>(pass.depthStencilAttachment.value())
                    .stencilAttachment = attachment;
            return *this;
        }

        void execute(std::function<void(RasterContext &)> callback) {
            pass.callback = std::move(callback);
            buildCallback(std::move(pass));
        }

    private:
        std::function<void(RasterPass &&)> buildCallback;
        RasterPass pass;
    };
}

#endif //XENGINE_RASTERPASSBUILDER_HPP
