/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_WIREFRAMEPASS_HPP
#define XENGINE_WIREFRAMEPASS_HPP

#include <unordered_set>

#include "xng/resource/uri.hpp"

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/graph/framegraphtextureatlas.hpp"
#include "xng/render/scene/scene.hpp"
#include "xng/render/graph/meshallocator.hpp"

namespace xng {
    /**
     * The WireframePass draws the wire frame of the geometry in the scene to the SCREEN_COLOR and SCREEN_DEPTH textures.
     *
     * Depth testing is performed with the existing contents of SCREEN_DEPTH.
     */
    class XENGINE_EXPORT WireframePass : public FrameGraphPass {
    public:
        ~WireframePass() override = default;

        void setup(FrameGraphBuilder &builder) override;

        std::type_index getTypeIndex() const override;

    private:
        FrameGraphResource renderPipeline;

        FrameGraphResource vertexBuffer;
        FrameGraphResource indexBuffer;

        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};

        MeshAllocator meshAllocator;
    };
}

#endif //XENGINE_WIREFRAMEPASS_HPP
