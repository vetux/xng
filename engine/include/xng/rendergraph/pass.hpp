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

#ifndef XENGINE_RENDERGRAPH_PASS_HPP
#define XENGINE_RENDERGRAPH_PASS_HPP

#include <string>
#include <functional>
#include <unordered_set>

#include "xng/rendergraph/resourceid.hpp"
#include "xng/rendergraph/pipelinecache.hpp"
#include "xng/rendergraph/attachment.hpp"

#include "xng/rendergraph/context/rastercontext.hpp"
#include "xng/rendergraph/context/transfercontext.hpp"
#include "xng/rendergraph/context/computecontext.hpp"

#include "xng/rendergraph/texturebinding.hpp"

//TODO: Implement texture image binding + shader IR support (TextureStorageRead/Write)

namespace xng::rg {
    struct BufferAccess {
        enum Type {
            StorageRead,
            StorageWrite,
            VertexRead,
            IndexRead,
            IndirectRead,
            TransferSrc,
            TransferDst,
        };

        Type type{};
        size_t offset = 0;
        size_t size = 0; // 0 = Whole buffer / offset to end of buffer

        BufferAccess(const Type type, const size_t offset, const size_t size)
            : type(type), offset(offset), size(size) {
        }
    };

    struct TextureAccess {
        enum Type {
            TextureAttachmentColor,
            TextureAttachmentDepthStencil,
            TextureSampledRead,
            TextureStorageRead,
            TextureStorageWrite,
            TextureTransferSrc,
            TextureTransferDst,
        };

        Type type{};
        TextureBinding::Range range{};
        TextureBinding::Aspect aspect{};

        TextureAccess(const Type type, const TextureBinding::Range range, const TextureBinding::Aspect aspect)
            : type(type), range(range), aspect(aspect) {
        }
    };

    /**
     * WAW / WAR hazards are undefined behavior.
     * Check the Graph Comment for more information.
     *
     * @tparam T
     */
    template<typename T>
    struct ResourceAccess {
        std::vector<T> entries;
    };

    template<typename T>
    struct RenderResourceAccess {
        struct Entry {
            T access; // The access description
            std::unordered_set<Shader::Stage> stages; // The pipeline stages in which the access happens.

            Entry(std::unordered_set<Shader::Stage> stage, T access)
                : access(std::move(access)), stages(std::move(stage)) {
            }

            Entry(const Shader::Stage stage, T access)
                : access(std::move(access)), stages({stage}) {
            }

            /**
             * For *Transfer* accesses and TextureAttachment* accesses no stages are declared.
             * @param access
             */
            explicit Entry(T access)
                : access(std::move(access)) {
            }
        };

        std::vector<Entry> entries;
    };

    /**
     * Transfer passes can only perform *Transfer* accesses
     * and run on a dedicated transfer queue if available.
     */
    struct TransferPass {
        std::string name;
        std::function<void(TransferContext &)> callback; // May be invoked on a different thread.

        std::unordered_map<ResourceId, ResourceAccess<BufferAccess>, ResourceIdHash> bufferUsages;
        std::unordered_map<ResourceId, ResourceAccess<TextureAccess>, ResourceIdHash> textureUsages;
    };

    /**
     * Compute passes can only perform *Read, *Write and *StorageRead/Write accesses
     * and run on a dedicated compute queue if available.
     */
    struct ComputePass {
        std::string name;
        std::function<void(ComputeContext &)> callback; // May be invoked on a different thread.

        std::unordered_map<ResourceId, ResourceAccess<BufferAccess>, ResourceIdHash> bufferUsages;
        std::unordered_map<ResourceId, ResourceAccess<TextureAccess>, ResourceIdHash> textureUsages;
    };

    /**
     * Graphics passes can access all context types and run on a dedicated graphics queue if available.
     */
    struct GraphicsPass {
        //TODO: Sub Render Pass interface (Mobile Tiling)

        std::string name;
        std::function<void(RasterContext &,
                           TransferContext &,
                           ComputeContext &)> callback; // May be invoked on a different thread.

        std::unordered_map<ResourceId, RenderResourceAccess<BufferAccess>, ResourceIdHash> bufferUsages;
        std::unordered_map<ResourceId, RenderResourceAccess<TextureAccess>, ResourceIdHash> textureUsages;
        std::unordered_map<std::shared_ptr<Surface>, ResourceAccess<TextureAccess> > surfaceUsages;
    };

    typedef std::variant<TransferPass, ComputePass, GraphicsPass> Pass;
}

#endif //XENGINE_RENDERGRAPH_PASS_HPP
