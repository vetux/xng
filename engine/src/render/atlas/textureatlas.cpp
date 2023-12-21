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

#include "xng/render/atlas/textureatlas.hpp"

namespace xng {
    ImageRGBA TextureAtlas::getAlignedImage(const ImageRGBA &texture, TextureAtlasResolution res) {
        auto size = getResolutionLevelSize(res);
        ImageRGBA image(size);
        image.blit(Vec2i(0, 0), texture);
        return std::move(image);
    }

    void TextureAtlas::upload(const TextureAtlasHandle &handle,
                              const std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> &atlasBuffers,
                              const ImageRGBA &texture) {
        atlasBuffers.at(handle.level).get().upload(handle.index, getAlignedImage(texture, handle.level));
    }

    void TextureAtlas::upload(FrameGraphBuilder &builder,
                              const TextureAtlasHandle &handle,
                              const std::map<TextureAtlasResolution, FrameGraphResource> &atlasBuffers,
                              const ImageRGBA &texture) {
        auto img = getAlignedImage(texture, handle.level);
        builder.upload(atlasBuffers.at(handle.level),
                       handle.index,
                       0,
                       RGBA,
                       {},
                       [texture, handle]() {
                           return FrameGraphUploadBuffer::createArray(getAlignedImage(texture, handle.level).getBuffer());
                       });
    }

    TextureAtlas::TextureAtlas(std::map<TextureAtlasResolution, std::vector<bool>> bufferOccupations)
            : bufferOccupations(std::move(bufferOccupations)) {}

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
        throw std::runtime_error("No free slot in atlas texture array buffers.");
    }

    void TextureAtlas::remove(const TextureAtlasHandle &handle) {
        if (!bufferOccupations.at(handle.level).at(handle.index)) {
            throw std::runtime_error("Texture already removed");
        }
        bufferOccupations.at(handle.level).at(handle.index) = false;
    }
}