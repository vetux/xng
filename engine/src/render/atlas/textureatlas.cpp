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

#include "xng/render/atlas/textureatlas.hpp"

namespace xng {
    ImageRGBA TextureAtlas::getAlignedImage(const ImageRGBA &texture, TextureAtlasResolution res) {
        auto size = getResolutionLevelSize(res);
        ImageRGBA image(size);
        image.blit(Vec2i(0, 0), texture);
        return std::move(image);
    }

    void TextureAtlas::upload(RenderGraphContext &ctx, const TextureAtlasHandle &handle,
                              const std::map<TextureAtlasResolution, RenderGraphResource> &atlasBuffers,
                              const ImageRGBA &texture) {
        auto img = getAlignedImage(texture, handle.level);
        ctx.uploadTexture(atlasBuffers.at(handle.level),
                          reinterpret_cast<const uint8_t *>(img.getBuffer().data()),
                          img.getBuffer().size() * sizeof(ColorRGBA),
                          RGBA);
    }

    TextureAtlas::TextureAtlas(std::map<TextureAtlasResolution, std::vector<bool> > bufferOccupations)
        : bufferOccupations(std::move(bufferOccupations)) {
    }

    TextureAtlasHandle TextureAtlas::add(const ImageRGBA &texture) {
        auto res = getClosestMatchingResolutionLevel(texture.getResolution());
        for (size_t i = 0; i < bufferOccupations[res].size(); i++) {
            if (!bufferOccupations.at(res).at(i)) {
                bufferOccupations.at(res).at(i) = !bufferOccupations.at(res).at(i);
                TextureAtlasHandle ret;
                ret.index = i;
                ret.level = res;
                ret.size = texture.getResolution();
                return ret;
            }
        }
        // No free slot, increase texture atlas array texture size
        bufferOccupations.at(res).emplace_back(true);
        TextureAtlasHandle ret;
        ret.index = bufferOccupations.size() - 1;
        ret.level = res;
        ret.size = texture.getResolution();
        return ret;
    }

    void TextureAtlas::remove(const TextureAtlasHandle &handle) {
        if (!bufferOccupations.at(handle.level).at(handle.index)) {
            throw std::runtime_error("Texture already removed");
        }
        bufferOccupations.at(handle.level).at(handle.index) = false;
    }
}
