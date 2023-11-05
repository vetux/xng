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
#include "xng/render/meshallocator.hpp"

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

        void execute(FrameGraphPassResources &resources,
                     const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                     const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                     const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) override;

        std::type_index getTypeIndex() const override;

    private:
        TextureAtlasHandle getTexture(const ResourceHandle <Texture> &texture,
                                      std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> &atlasBuffers);

        void deallocateTexture(const ResourceHandle<Texture> &texture);

        FrameGraphResource forwardColorRes;
        FrameGraphResource forwardDepthRes;

        FrameGraphResource deferredDepthRes;

        FrameGraphResource targetRes;
        FrameGraphResource pipelineRes;
        FrameGraphResource passRes;
        FrameGraphResource shaderBufferRes;

        FrameGraphResource pointLightBufferRes;
        FrameGraphResource shadowPointLightBufferRes;

        FrameGraphResource vertexBufferRes;
        FrameGraphResource indexBufferRes;
        FrameGraphResource vertexArrayObjectRes;

        FrameGraphResource staleVertexBuffer;
        FrameGraphResource staleIndexBuffer;

        FrameGraphResource commandBuffer;

        FrameGraphResource pointLightShadowMapRes;
        FrameGraphResource defPointShadowMap;

        FrameGraphTextureAtlas atlas;

        Camera camera;
        Transform cameraTransform;

        Vec2i renderSize;

        Scene scene;

        std::vector<Node> nodes;

        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};

        MeshAllocator meshAllocator;

        std::map<Uri, TextureAtlasHandle> textures;

        std::set<Uri> usedTextures;
        std::set<Uri> usedMeshes;

        bool bindVao = true;

        size_t drawCycles;
    };
}
#endif //XENGINE_FORWARDLIGHTINGPASS_HPP
