/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/render/meshallocator.hpp"
#include "xng/render/renderconfiguration.hpp"
#include "xng/render/renderpass.hpp"
#include "xng/render/renderregistry.hpp"
#include "xng/render/atlas/textureatlas.hpp"

namespace xng {
    class XENGINE_EXPORT ConstructionPass final : public RenderPass {
    public:
        ConstructionPass(std::shared_ptr<RenderConfiguration> config,
                         std::shared_ptr<RenderRegistry> registry)
            : config(std::move(config)),
              registry(std::move(registry)) {
        }

        ~ConstructionPass() override = default;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

        bool shouldRebuild() override;

    private:
        void runPass(RenderGraphContext &ctx);

        static Shader createVertexShader();

        static Shader createSkinnedVertexShader();

        static Shader createFragmentShader();

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<RenderRegistry> registry;

        RenderGraphResource renderPipelineSkinned;

        RenderGraphResource gBufferPosition;
        RenderGraphResource gBufferNormal;
        RenderGraphResource gBufferTangent;
        RenderGraphResource gBufferRoughnessMetallicAmbientOcclusion;
        RenderGraphResource gBufferAlbedo;
        RenderGraphResource gBufferObjectShadows;
        RenderGraphResource gBufferDepth;

        RenderGraphResource shaderBuffer;
        RenderGraphResource boneBuffer;

        TextureAtlas atlas;

        size_t currentShaderBufferSize{};
        size_t currentBoneBufferSize{};

        size_t totalShaderBufferSize{};
        size_t totalBoneBufferSize{};

        MeshAllocator meshAllocator;

        std::map<Uri, TextureAtlasHandle> textures;
        std::set<ResourceHandle<SkinnedMesh> > meshes;

        Vec2i resolution;

        Transform cameraTransform;
        Camera camera;
        std::vector<Node> objects;
    };
}

#endif //XENGINE_CONSTRUCTIONPASS_HPP
