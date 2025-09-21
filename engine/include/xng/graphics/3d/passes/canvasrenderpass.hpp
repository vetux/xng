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

#ifndef XENGINE_CANVASRENDERPASS_HPP
#define XENGINE_CANVASRENDERPASS_HPP

#include "xng/graphics/3d/meshbuffer2d.hpp"
#include "xng/graphics/3d/renderpass.hpp"

#include "xng/graphics/3d/renderconfiguration.hpp"
#include "xng/graphics/3d/sharedresourceregistry.hpp"
#include "xng/graphics/3d/atlas/textureatlas.hpp"
#include "xng/graphics/3d/atlas/textureatlashandle.hpp"
#include "xng/graphics/3d/sharedresources/compositinglayers.hpp"

namespace xng {
    /**
     * Renders canvases to textures and presents these textures either in the world or screen space.
     *
     * Text glyphs are allocated as they are encountered in the canvases and kept allocated as long as the font / pixel size
     * they belong to is used in the current canvases.
     */
    class XENGINE_EXPORT CanvasRenderPass final : public RenderPass {
    public:
        CanvasRenderPass(std::shared_ptr<RenderConfiguration> config, std::shared_ptr<SharedResourceRegistry> registry);

        bool shouldRebuild(const Vec2i &backBufferSize) override;

        void create(RenderGraphBuilder &builder) override;

        void recreate(RenderGraphBuilder &builder) override;

    private:
        void runPass(RenderGraphContext &ctx);

        void updateGlyphTextures();

        void updateTextures();

        void renderCanvas(RenderGraphResource target,
                          const Canvas &canvas,
                          RenderGraphContext &ctx);

        static Shader createVertexShader();

        static Shader createFragmentShader();

        std::shared_ptr<RenderConfiguration> config;
        std::shared_ptr<SharedResourceRegistry> registry;

        CompositeLayer screenSpaceLayer;
        CompositeLayer worldSpaceLayer;
        Vec2i layerSize;

        std::vector<Canvas> canvases;

        RenderGraphResource trianglePipeline;
        RenderGraphResource linePipeline;
        RenderGraphResource pointPipeline;

        RenderGraphResource shaderBuffer;

        RenderGraphResource vertexBuffer;
        RenderGraphResource indexBuffer;

        RenderGraphResource vertexBufferCopy;
        RenderGraphResource indexBufferCopy;

        size_t vertexBufferSize{};
        size_t indexBufferSize{};

        MeshBuffer2D meshBuffer;

        // TODO: Share a single atlas instance across the passes.
        TextureAtlas atlas;
        std::map<Uri, TextureAtlasHandle> textureAtlasHandles;

        class GlyphTextureHash {
        public:
            std::size_t operator()(const std::pair<Uri, Vec2i> &k) const {
                size_t ret = 0;
                hash_combine(ret, k.first.toString());
                hash_combine(ret, k.second.x);
                hash_combine(ret, k.second.y);
                return ret;
            }
        };

        std::unordered_map<std::pair<Uri, Vec2i>, std::map<char, TextureAtlasHandle>, GlyphTextureHash> glyphTextures;
    };
}

#endif //XENGINE_CANVASRENDERPASS_HPP
