/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_SHADOWMAPPINGPASS_HPP
#define XENGINE_SHADOWMAPPINGPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/scene/pointlight.hpp"
#include "xng/render/graph/meshallocator.hpp"
#include "xng/render/scene/scene.hpp"

namespace xng {
    /**
     * The shadow mapping pass creates the shadow mapping textures.
     *
     * Writes SLOT_SHADOW_MAP_*
     */
    class XENGINE_EXPORT ShadowMappingPass : public FrameGraphPass {
    public:
        ~ShadowMappingPass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        std::type_index getTypeIndex() const override;

    private:
        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};

        MeshAllocator meshAllocator;

        FrameGraphResource pointPipeline;
        FrameGraphResource dirPipeline;

        FrameGraphResource vertexBuffer;
        FrameGraphResource indexBuffer;

        FrameGraphResource staleVertexBuffer;
        FrameGraphResource staleIndexBuffer;
    };
}
#endif //XENGINE_SHADOWMAPPINGPASS_HPP
