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

#ifndef XENGINE_RASTERPASSBUILDER_HPP
#define XENGINE_RASTERPASSBUILDER_HPP

#include <functional>

#include "xng/rendergraph/pass.hpp"

namespace xng::rg {
    class GraphicsPassBuilder {
    public:
        explicit GraphicsPassBuilder(std::string name) {
            pass.name = std::move(name);
        }

        GraphicsPassBuilder &vertexRead(const Resource<Buffer> &buffer,
                                        const size_t offset = 0,
                                        const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::VertexRead, offset, size);
            const auto entry = GraphicsResourceAccess<BufferAccess>::Entry(Shader::VERTEX, access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &indexRead(const Resource<Buffer> &buffer,
                                       const size_t offset = 0,
                                       const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::IndexRead, offset, size);
            const auto entry = GraphicsResourceAccess<BufferAccess>::Entry(Shader::VERTEX, access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &storageRead(const Resource<Buffer> &buffer,
                                         std::unordered_set<Shader::Stage> stages,
                                         const size_t offset = 0,
                                         const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::StorageRead, offset, size);
            const auto entry = GraphicsResourceAccess<BufferAccess>::Entry(std::move(stages), access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &storageWrite(const Resource<Buffer> &buffer,
                                          std::unordered_set<Shader::Stage> stages,
                                          const size_t offset = 0,
                                          const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::StorageWrite, offset, size);
            const auto entry = GraphicsResourceAccess<BufferAccess>::Entry(std::move(stages), access);
            pass.bufferUsages[buffer].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &textureStorageRead(const Resource<Texture> &texture,
                                                std::unordered_set<Shader::Stage> stages,
                                                const TextureBinding::Range range = {},
                                                const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureStorageRead,
                                              range,
                                              aspect);
            const auto entry = GraphicsResourceAccess<TextureAccess>::Entry(std::move(stages), access);
            pass.textureUsages[texture].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &textureStorageWrite(const Resource<Texture> &texture,
                                                 std::unordered_set<Shader::Stage> stages,
                                                 const TextureBinding::Range range = {},
                                                 const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureStorageWrite,
                                              range,
                                              aspect);
            const auto entry = GraphicsResourceAccess<TextureAccess>::Entry(std::move(stages), access);
            pass.textureUsages[texture].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &textureSampledRead(const Resource<Texture> &texture,
                                                std::unordered_set<Shader::Stage> stages,
                                                const TextureBinding::Range range = {},
                                                const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureSampledRead,
                                              range,
                                              aspect);
            const auto entry = GraphicsResourceAccess<TextureAccess>::Entry(std::move(stages), access);
            pass.textureUsages[texture].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &textureAttachmentColor(const Resource<Texture> &texture,
                                                    const TextureBinding::Range range = {},
                                                    const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureAttachmentColor,
                                              range,
                                              aspect);
            const auto entry = GraphicsResourceAccess<TextureAccess>::Entry({Shader::VERTEX, Shader::FRAGMENT},
                                                                          access);
            pass.textureUsages[texture].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &textureAttachmentDepthStencil(const Resource<Texture> &texture,
                                                           const TextureBinding::Range range = {},
                                                           const TextureBinding::Aspect aspect =
                                                                   TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureAttachmentDepthStencil,
                                              range,
                                              aspect);
            const auto entry = GraphicsResourceAccess<TextureAccess>::Entry({Shader::VERTEX, Shader::FRAGMENT},
                                                                          access);
            pass.textureUsages[texture].entries.emplace_back(entry);
            return *this;
        }

        GraphicsPassBuilder &surfaceAttachmentColor(const std::shared_ptr<Surface> &surface) {
            const auto access = TextureAccess(TextureAccess::TextureAttachmentColor,
                                              {},
                                              TextureBinding::Automatic);
            pass.surfaceUsages[surface].entries.emplace_back(access);
            return *this;
        }

        GraphicsPassBuilder &surfaceAttachmentDepthStencil(const std::shared_ptr<Surface> &surface) {
            const auto access = TextureAccess(TextureAccess::TextureAttachmentDepthStencil,
                                              {},
                                              TextureBinding::Automatic);
            pass.surfaceUsages[surface].entries.emplace_back(access);
            return *this;
        }

        GraphicsPassBuilder &transferRead(const Resource<Buffer> &buffer,
                                          const size_t offset = 0,
                                          const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::TransferSrc, offset, size);
            pass.bufferUsages[buffer].entries.emplace_back(access);
            return *this;
        }

        GraphicsPassBuilder &transferWrite(const Resource<Buffer> &buffer, const size_t offset = 0,
                                           const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::TransferDst, offset, size);
            pass.bufferUsages[buffer].entries.emplace_back(access);
            return *this;
        }

        GraphicsPassBuilder &transferRead(const Resource<Texture> &texture,
                                          const TextureBinding::Range range = {},
                                          const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureTransferSrc,
                                              range,
                                              aspect);
            pass.textureUsages[texture].entries.emplace_back(access);
            return *this;
        }

        GraphicsPassBuilder &transferWrite(const Resource<Texture> &texture,
                                           const TextureBinding::Range range = {},
                                           const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureTransferDst,
                                              range,
                                              aspect);
            pass.textureUsages[texture].entries.emplace_back(access);
            return *this;
        }

        const GraphicsPass &execute(std::function<void(RasterContext &,
                                                       TransferContext &,
                                                       ComputeContext &)> callback) {
            pass.callback = std::move(callback);
            return pass;
        }

    private:
        GraphicsPass pass;
    };
}

#endif //XENGINE_RASTERPASSBUILDER_HPP
