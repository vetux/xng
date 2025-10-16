/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_TEXTUREATLAS_HPP
#define XENGINE_TEXTUREATLAS_HPP

#include "xng/graphics/textureatlas/textureatlashandle.hpp"

#include "xng/rendergraph/rendergraphcontext.hpp"
#include "xng/rendergraph/rendergraphbuilder.hpp"

namespace xng {
    /**
     * A texture atlas abstraction for render graph.
     *
     * The texture atlas contains an array texture for each TextureAtlasResolution.
     *
     * When a texture is added, it is added to the array texture with the closest matching or larger size.
     *
     * The texture is arranged to the bottom left so uv coordinates can simply be scaled with the ratio of the size
     * of the texture compared to the size of the array buffer.
     *
     * There are 12 array textures in total, which leaves 4 more texture buffer bindings free to use for other things.
     */
    class XENGINE_EXPORT TextureAtlas {
    public:
        static Vec2i getResolutionLevelSize(TextureAtlasResolution level);

        static TextureAtlasResolution getClosestMatchingResolutionLevel(const Vec2i &size);

        TextureAtlas();

        explicit TextureAtlas(std::map<TextureAtlasResolution, std::vector<bool> > bufferOccupations);

        TextureAtlasHandle add(const ImageRGBA &texture);

        void remove(const TextureAtlasHandle &handle);

        void declareReadWrite(RenderGraphBuilder &builder, RenderGraphBuilder::PassHandle pass) const;

        bool shouldRebuild();

        void onCreate(RenderGraphBuilder &builder);

        void onRecreate(RenderGraphBuilder &builder);

        const std::unordered_map<TextureAtlasResolution, RenderGraphResource> &
        getAtlasTextures(RenderGraphContext &ctx);

    private:
        static void upload(RenderGraphContext &ctx,
                           const TextureAtlasHandle &handle,
                           const std::unordered_map<TextureAtlasResolution, RenderGraphResource> &atlasBuffers,
                           const ImageRGBA &texture);

        size_t getFreeSlotCount(TextureAtlasResolution resolution);

        std::map<TextureAtlasResolution, std::vector<bool> > bufferOccupations;

        std::unordered_map<TextureAtlasResolution, RenderGraphResource> currentHandles;

        std::unordered_map<TextureAtlasResolution, RenderGraphResource> copyHandles;
        std::unordered_map<TextureAtlasResolution, size_t> copySizes;

        std::map<TextureAtlasResolution, size_t> bufferSizes;

        std::unordered_map<TextureAtlasHandle, ImageRGBA> pendingTextures;
    };
}
#endif //XENGINE_TEXTUREATLAS_HPP
