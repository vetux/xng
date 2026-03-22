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

#ifndef XENGINE_TRANSFERPASSBUILDER_HPP
#define XENGINE_TRANSFERPASSBUILDER_HPP

#include <functional>

#include "xng/rendergraph/pass.hpp"

namespace xng::rendergraph {
    class TransferPassBuilder {
    public:
        TransferPassBuilder(std::string name, std::function<void(TransferPass &&)> buildCallback)
            : buildCallback(std::move(buildCallback)) {
            pass.name = std::move(name);
        }

        TransferPassBuilder &read(const Resource<Buffer> &buffer, const size_t offset, const size_t size) {
            const auto access = BufferAccess(BufferAccess::TransferSrc, offset, size);
            pass.bufferUsages[buffer].entries.emplace_back(access);
            return *this;
        }

        TransferPassBuilder &write(const Resource<Buffer> &buffer, const size_t offset, const size_t size) {
            const auto access = BufferAccess(BufferAccess::TransferDst, offset, size);
            pass.bufferUsages[buffer].entries.emplace_back(access);
            return *this;
        }

        TransferPassBuilder &read(const Resource<Texture> &texture,
                                  const TextureBinding::Range range = {},
                                  const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureTransferSrc,
                                              range,
                                              aspect);
            pass.textureUsages[texture].entries.emplace_back(access);
            return *this;
        }

        TransferPassBuilder &write(const Resource<Texture> &texture,
                                   const TextureBinding::Range range = {},
                                   const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureTransferDst,
                                              range,
                                              aspect);
            pass.textureUsages[texture].entries.emplace_back(access);
            return *this;
        }

        void execute(std::function<void(TransferContext &)> callback) {
            pass.callback = std::move(callback);
            buildCallback(std::move(pass));
        }

    private:
        std::function<void(TransferPass &&)> buildCallback;
        TransferPass pass;
    };
}

#endif //XENGINE_TRANSFERPASSBUILDER_HPP
