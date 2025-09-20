/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/graphics/3d/atlas/textureatlas.hpp"

namespace xng {
    ImageRGBA TextureAtlas::getAlignedImage(const ImageRGBA &texture, TextureAtlasResolution res) {
        auto size = getResolutionLevelSize(res);
        ImageRGBA image(size, ColorRGBA::black(1, 0));
        image.blit(Vec2i(0, 0), texture);
        return std::move(image);
    }

    void TextureAtlas::upload(RenderGraphContext &ctx,
                              const TextureAtlasHandle &handle,
                              const std::unordered_map<TextureAtlasResolution, RenderGraphResource> &atlasBuffers,
                              const ImageRGBA &texture) {
        auto img = getAlignedImage(texture, handle.level);
        ctx.uploadTexture(atlasBuffers.at(handle.level),
                          reinterpret_cast<const uint8_t *>(img.getBuffer().data()),
                          img.getBuffer().size() * sizeof(ColorRGBA),
                          RGBA,
                          handle.index);
    }

    TextureAtlas::TextureAtlas(std::map<TextureAtlasResolution, std::vector<bool> > bufferOccupations)
        : bufferOccupations(std::move(bufferOccupations)) {
    }

    TextureAtlasHandle TextureAtlas::add(const ImageRGBA &texture) {
        TextureAtlasHandle ret;
        bool gotHandle = false;

        const auto res = getClosestMatchingResolutionLevel(texture.getResolution());
        for (size_t i = 0; i < bufferOccupations.at(res).size(); i++) {
            if (!bufferOccupations.at(res).at(i)) {
                bufferOccupations.at(res).at(i) = true;
                ret.index = i;
                ret.level = res;
                ret.size = texture.getResolution();
                gotHandle = true;
                break;
            }
        }

        if (!gotHandle) {
            // No free slot, increase texture atlas array texture size
            bufferOccupations.at(res).emplace_back(true);
            ret.index = bufferOccupations.at(res).size() - 1;
            ret.level = res;
            ret.size = texture.getResolution();
        }

        pendingTextures[ret] = texture;

        return ret;
    }

    void TextureAtlas::remove(const TextureAtlasHandle &handle) {
        if (!bufferOccupations.at(handle.level).at(handle.index)) {
            throw std::runtime_error("Texture already removed");
        }
        bufferOccupations.at(handle.level).at(handle.index) = false;
    }

    Vec2i TextureAtlas::getResolutionLevelSize(TextureAtlasResolution level) {
        switch (level) {
            case TEXTURE_ATLAS_8x8:
                return {8, 8};
            case TEXTURE_ATLAS_16x16:
                return {16, 16};
            case TEXTURE_ATLAS_32x32:
                return {32, 32};
            case TEXTURE_ATLAS_64x64:
                return {64, 64};
            case TEXTURE_ATLAS_128x128:
                return {128, 128};
            case TEXTURE_ATLAS_256x256:
                return {256, 256};
            case TEXTURE_ATLAS_512x512:
                return {512, 512};
            case TEXTURE_ATLAS_1024x1024:
                return {1024, 1024};
            case TEXTURE_ATLAS_2048x2048:
                return {2048, 2048};
            case TEXTURE_ATLAS_4096x4096:
                return {4096, 4096};
            case TEXTURE_ATLAS_8192x8192:
                return {8192, 8192};
            case TEXTURE_ATLAS_16384x16384:
                return {16384, 16384};
            default:
                throw std::runtime_error("Invalid texture atlas resolution level");
        }
    }

    TextureAtlasResolution TextureAtlas::getClosestMatchingResolutionLevel(const Vec2i &size) {
        for (auto i = (int) TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
            auto res = getResolutionLevelSize((TextureAtlasResolution) i);
            if (size.x <= res.x && size.y <= res.y) {
                return (TextureAtlasResolution) i;
            }
        }
        throw std::runtime_error("No matching resolution level found");
    }

    TextureAtlas::TextureAtlas() {
        for (int i = TEXTURE_ATLAS_8x8; i < TEXTURE_ATLAS_END; i++) {
            auto res = static_cast<TextureAtlasResolution>(i);
            bufferOccupations[res] = {};
        }
    }

    void TextureAtlas::declareReadWrite(RenderGraphBuilder &builder, RenderGraphBuilder::PassHandle pass) const {
        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
            builder.readWrite(pass, currentHandles.at(static_cast<TextureAtlasResolution>(i)));
        }
        if (previousHandles.size() > 0) {
            for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
                builder.readWrite(pass, previousHandles.at(static_cast<TextureAtlasResolution>(i)));
            }
        }
    }

    bool TextureAtlas::shouldRebuild() {
        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
            const auto res = static_cast<TextureAtlasResolution>(i);
            if (bufferSizes.at(res) < bufferOccupations.at(res).size()) {
                return true;
            }
        }
        return false;
    }

    void TextureAtlas::onCreate(RenderGraphBuilder &builder) {
        RenderGraphTexture desc;
        desc.isArrayTexture = true;

        for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
            const auto res = static_cast<TextureAtlasResolution>(i);
            desc.arrayLayers = bufferOccupations.at(res).size();
            desc.size = getResolutionLevelSize(res);
            currentHandles[res] = builder.createTexture(desc);
            bufferSizes[res] = 0;
        }
    }

    void TextureAtlas::onRecreate(RenderGraphBuilder &builder) {
        for (auto &pair: bufferOccupations) {
            if (bufferSizes[pair.first] < pair.second.size()) {
                if (bufferSizes[pair.first] > 0) {
                    previousHandles[pair.first] = builder.inheritResource(currentHandles.at(pair.first));
                }
                bufferSizes[pair.first] = pair.second.size();
                RenderGraphTexture desc;
                desc.isArrayTexture = true;
                desc.arrayLayers = pair.second.size();
                desc.size = getResolutionLevelSize(pair.first);
                currentHandles[pair.first] = builder.createTexture(desc);
            } else {
                currentHandles[pair.first] = builder.inheritResource(currentHandles.at(pair.first));
            }
        }
    }

    const std::unordered_map<TextureAtlasResolution, RenderGraphResource> &
    TextureAtlas::getAtlasTextures(RenderGraphContext &ctx) {
        if (previousHandles.size() > 0) {
            for (int i = TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
                const auto res = static_cast<TextureAtlasResolution>(i);
                if (currentHandles.at(res) != previousHandles.at(res)) {
                    ctx.copyTexture(currentHandles.at(res), previousHandles.at(res));
                }
            }
            previousHandles.clear();
        }

        for (auto &tex: pendingTextures) {
            upload(ctx, tex.first, currentHandles, tex.second);
        }
        pendingTextures.clear();

        return currentHandles;
    }

    size_t TextureAtlas::getFreeSlotCount(TextureAtlasResolution resolution) {
        size_t ret = 0;
        for (auto slot: bufferOccupations[resolution]) {
            if (!slot)
                ret++;
        }
        return ret;
    }
}
