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

#ifndef XENGINE_COMPUTEPASSBUILDER_HPP
#define XENGINE_COMPUTEPASSBUILDER_HPP

#include <functional>

#include "xng/rendergraph/pass.hpp"

namespace xng::rendergraph {
    class ComputePassBuilder {
    public:
        ComputePassBuilder(std::string name, std::function<void(ComputePass &&)> buildCallback)
            : buildCallback(std::move(buildCallback)) {
            pass.name = std::move(name);
        }

        ComputePassBuilder &read(const Resource<StorageBuffer> &buffer,
                                 const size_t offset = 0,
                                 const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::StorageRead, offset, size);
            pass.bufferUsages[buffer].entries.emplace_back(access);
            return *this;
        }

        ComputePassBuilder &write(const Resource<StorageBuffer> &buffer,
                                  const size_t offset = 0,
                                  const size_t size = 0) {
            const auto access = BufferAccess(BufferAccess::StorageWrite, offset, size);
            pass.bufferUsages[buffer].entries.emplace_back(access);
            return *this;
        }

        ComputePassBuilder &read(const Resource<Texture> &texture,
                                 const TextureBinding::Range range = {},
                                 const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureStorageRead,
                                              range,
                                              aspect);
            pass.textureUsages[texture].entries.emplace_back(access);
            return *this;
        }

        ComputePassBuilder &write(const Resource<Texture> &texture,
                                  const TextureBinding::Range range = {},
                                  const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureStorageWrite,
                                              range,
                                              aspect);
            pass.textureUsages[texture].entries.emplace_back(access);
            return *this;
        }

        ComputePassBuilder &sample(const Resource<Texture> &texture,
                                   const TextureBinding::Range range = {},
                                   const TextureBinding::Aspect aspect = TextureBinding::Automatic) {
            const auto access = TextureAccess(TextureAccess::TextureSampledRead,
                                              range,
                                              aspect);
            pass.textureUsages[texture].entries.emplace_back(access);
            return *this;
        }

        void execute(std::function<void(ComputeContext &)> callback) {
            pass.callback = std::move(callback);
            buildCallback(std::move(pass));
        }

    private:
        std::function<void(ComputePass &&)> buildCallback;
        ComputePass pass;
    };
}

#endif //XENGINE_COMPUTEPASSBUILDER_HPP
