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

namespace xng::rendergraph {
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
     * Overlapping sub resource accesses in a single pass are undefined behavior.
     *
     * The runtime may catch this in the execute() call and throw an exception.
     *
     * Performing non overlapping TextureStorageRead + TextureStorageWrite on a texture forces the runtime
     * to use a compatible layout and should therefore be avoided.
     *
     * @tparam T
     */
    template<typename T>
    struct ResourceAccess {
        std::vector<T> entries;
    };

    template<typename T>
    struct RasterResourceAccess {
        struct Entry {
            T access; // The access description
            std::unordered_set<Shader::Stage> stage; // The raster pipeline stages in which the access happens.

            Entry(std::unordered_set<Shader::Stage> stage, T access)
                : access(std::move(access)), stage(std::move(stage)) {
            }

            Entry(const Shader::Stage stage, T access)
                : access(std::move(access)), stage({stage}) {
            }
        };

        std::vector<Entry> entries;
    };

    /**
     * Transfer passes can only perform *Transfer* accesses.
     */
    struct TransferPass {
        std::string name;
        std::function<void(TransferContext &)> callback;

        std::unordered_map<ResourceId, ResourceAccess<BufferAccess>, ResourceIdHash> bufferUsages;
        std::unordered_map<ResourceId, ResourceAccess<TextureAccess>, ResourceIdHash> textureUsages;
    };

    /**
     * Raster passes can only perform *Read, *Write and TextureSampledRead accesses.
     */
    struct RasterPass {
        struct DepthStencilAttachment {
            Attachment depthAttachment;
            Attachment stencilAttachment;
        };

        std::string name;
        std::function<void(RasterContext &)> callback;

        std::unordered_map<ResourceId, RasterResourceAccess<BufferAccess>, ResourceIdHash> bufferUsages;
        std::unordered_map<ResourceId, RasterResourceAccess<TextureAccess>, ResourceIdHash> textureUsages;

        /**
         * The attachments.
         *
         * Attachment target textures cannot be defined in textureUsage simultaneously.
         *
         * Each attachment target must be unique.
         * For layered attachments there may not be any other attachment target overlapping with the layered attachment.
         *
         * Combined depth stencil attachment is provided by a single Attachment object.
         */
        std::vector<Attachment> colorAttachments;
        std::optional<std::variant<Attachment, DepthStencilAttachment> > depthStencilAttachment;
    };

    /**
     * Compute passes can only perform *Read, *Write and *StorageRead/Write accesses.
     */
    struct ComputePass {
        std::string name;
        std::function<void(ComputeContext &)> callback;

        std::unordered_map<ResourceId, ResourceAccess<BufferAccess>, ResourceIdHash> bufferUsages;
        std::unordered_map<ResourceId, ResourceAccess<TextureAccess>, ResourceIdHash> textureUsages;
    };

    typedef std::variant<TransferPass, RasterPass, ComputePass> Pass;
}

#endif //XENGINE_RENDERGRAPH_PASS_HPP
