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

#ifndef XENGINE_TEXTUREATLAS_HPP
#define XENGINE_TEXTUREATLAS_HPP

#include "xng/gpu/texturearraybuffer.hpp"
#include "xng/gpu/renderdevice.hpp"

#include "xng/render/atlas/textureatlashandle.hpp"
#include "xng/render/texture.hpp"

namespace xng {
    /**
     * The texture atlas contains texture array buffers for TextureAtlasResolutions.
     *
     * When a texture is added it is added to the array buffer with the closest matching larger size.
     *
     * The texture is arranged to the bottom left so uv coordinates can simply be scaled with the ratio of the size
     * of the texture compared to the size of the array buffer.
     *
     * There are 12 texture array buffers in total which leaves 4 more texture buffer bindings free to use for other things.
     */
    class XENGINE_EXPORT TextureAtlas {
    public:
        static Vec2i getResolutionLevelSize(TextureAtlasResolution level) {
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

        static TextureAtlasResolution getClosestMatchingResolutionLevel(const Vec2i &size) {
            for (auto i = (int) TEXTURE_ATLAS_BEGIN; i < TEXTURE_ATLAS_END; i++) {
                auto res = getResolutionLevelSize((TextureAtlasResolution) i);
                if (size.x <= res.x && size.y <= res.y) {
                    return (TextureAtlasResolution) i;
                }
            }
            throw std::runtime_error("No matching resolution level found");
        }

        static ImageRGBA getAlignedImage(const ImageRGBA &texture, TextureAtlasResolution res);

        static void upload(const TextureAtlasHandle &handle,
                           const std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> &atlasBuffers,
                           const ImageRGBA &texture);

        TextureAtlas() = default;

        TextureAtlas(std::map<TextureAtlasResolution, std::vector<bool>> bufferOccupations);

        TextureAtlasHandle add(const ImageRGBA &texture);

        void remove(const TextureAtlasHandle &handle);

        const std::map<TextureAtlasResolution, std::vector<bool>> &getBufferOccupations() const {
            return bufferOccupations;
        }

        std::map<TextureAtlasResolution, std::vector<bool>> &getBufferOccupations() {
            return bufferOccupations;
        }

        size_t getFreeSlotCount(TextureAtlasResolution resolution) {
            size_t ret = 0;
            for (auto slot: bufferOccupations[resolution]) {
                if (!slot)
                    ret++;
            }
            return ret;
        }

    private:
        std::map<TextureAtlasResolution, std::vector<bool>> bufferOccupations;
    };
}
#endif //XENGINE_TEXTUREATLAS_HPP
