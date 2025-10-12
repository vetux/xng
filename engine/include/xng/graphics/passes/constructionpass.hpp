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

#ifndef XENGINE_CONSTRUCTIONPASS_HPP
#define XENGINE_CONSTRUCTIONPASS_HPP

#include <utility>

#include "xng/graphics/renderscene.hpp"
#include "xng/graphics/renderconfiguration.hpp"
#include "xng/graphics/renderpass.hpp"
#include "xng/graphics/sharedresourceregistry.hpp"
#include "xng/graphics/meshatlas/meshatlas.hpp"
#include "xng/graphics/textureatlas/textureatlas.hpp"
#include "xng/graphics/sharedresources/gbuffer.hpp"

namespace xng {
    class XENGINE_EXPORT ConstructionPass final : public RenderPass {
    public:
        ConstructionPass(std::shared_ptr<RenderConfiguration> config,
                         std::shared_ptr<SharedResourceRegistry> registry)
            : config(std::move(config)),
              registry(std::move(registry)) {
        }

        ~ConstructionPass() override = default;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        bool shouldRebuild(const Vec2i &backBufferSize) override;

    private:
        void runPass(RenderGraphContext &ctx);

        static Shader createVertexShader();

        static Shader createSkinnedVertexShader();

        static Shader createFragmentShader();

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        RenderGraphResource renderPipelineSkinned;

        GBuffer gBuffer;

        RenderGraphResource shaderBuffer;
        RenderGraphResource boneBuffer;

        TextureAtlas textureAtlas;
        MeshAtlas meshAtlas;

        std::map<Uri, TextureAtlasHandle> textures;
        std::vector<ResourceHandle<SkinnedModel> > allocatedMeshes;

        size_t currentBoneBufferSize{};
        size_t totalBoneBufferSize{};

        Vec2i currentResolution;

        Transform cameraTransform;
        Camera camera;
        std::vector<SkinnedModelObject> objects;
    };
}

#endif //XENGINE_CONSTRUCTIONPASS_HPP
