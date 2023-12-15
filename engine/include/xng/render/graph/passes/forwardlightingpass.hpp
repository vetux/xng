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

#ifndef XENGINE_FORWARDLIGHTINGPASS_HPP
#define XENGINE_FORWARDLIGHTINGPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/graph/framegraphtextureatlas.hpp"
#include "xng/render/graph/meshallocator.hpp"

#include "xng/render/atlas/textureatlas.hpp"

namespace xng {
    /**
     * The forward shading model implementation used for transparent objects.
     *
     * Writes SLOT_FORWARD_COLOR and SLOT_FORWARD_DEPTH.
     *
     * Reads SLOT_DEFERRED_DEPTH
     */
    class XENGINE_EXPORT ForwardLightingPass : public FrameGraphPass {
    public:
        void setup(FrameGraphBuilder &builder) override;

        std::type_index getTypeIndex() const override;

    private:
        TextureAtlasHandle getTexture(const ResourceHandle <Texture> &texture,
                                      std::map<TextureAtlasResolution, FrameGraphResource> &atlasBuffers);

        void deallocateTexture(const ResourceHandle<Texture> &texture);

        FrameGraphResource pipelineRes;

        FrameGraphResource vertexBufferRes;
        FrameGraphResource indexBufferRes;
        FrameGraphResource vertexArrayObjectRes;

        FrameGraphTextureAtlas atlas;

        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};

        MeshAllocator meshAllocator;

        std::map<Uri, TextureAtlasHandle> textures;

        bool bindVao = true;
    };
}
#endif //XENGINE_FORWARDLIGHTINGPASS_HPP
