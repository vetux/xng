/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_TEXTURESTREAMER_HPP
#define XENGINE_TEXTURESTREAMER_HPP

#include "xng/assets/image.hpp"
#include "xng/math/vector2.hpp"

#include "xng/rendergraph/heap.hpp"
#include "xng/rendergraph/resource/texture.hpp"

#include "xng/renderer/textureresolution.hpp"
#include "xng/renderer/stream/streambuffer.hpp"
#include "xng/renderer/stream/streamtexture.hpp"

namespace xng {
    /**
     * The texture streamer streams images to the gpu via a fixed set of StreamTextures.
     *
     * Each resolution level represents an array texture.
     *
     * The images are stored in the closest fitting texture resolution slot.
     * This means texture memory is wasted for images which are not of any given resolution slot size and there is a maximum image size.
     *
     * The array textures are RGBA8 format, shaders must manually decode sRGB color data.
     *
     * All hardware filtering configuration on the texture is set to NEAREST. Thus, the shaders must perform filtering
     * which incurs some gpu side overhead in return for reduced binding overhead (One bind per level per frame)
     * and custom filtering solutions which may not be available in the hardware samplers (Bicubic etc.).
     *
     * Alternatively, textures could be stored in giant TEXTURE_2D atlas textures.
     * This would allow the streamer to optimize cache locality and the amount of wasted texture memory by controlling the layout
     * and allows setting separate color formats per texture, which also reduces wasted texture memory.
     * (This appears to be the approach Unreal is using in their Virtual Texturing system)
     */
    class TextureStreamer {
    public:
        static Vec2i getTextureResolutionLevelSize(const TextureResolution level) {
            switch (level) {
                case RESOLUTION_8x8: return {8, 8};
                case RESOLUTION_16x16: return {16, 16};
                case RESOLUTION_32x32: return {32, 32};
                case RESOLUTION_64x64: return {64, 64};
                case RESOLUTION_128x128: return {128, 128};
                case RESOLUTION_256x256: return {256, 256};
                case RESOLUTION_512x512: return {512, 512};
                case RESOLUTION_1024x1024: return {1024, 1024};
                case RESOLUTION_2048x2048: return {2048, 2048};
                case RESOLUTION_4096x4096: return {4096, 4096};
                case RESOLUTION_8192x8192: return {8192, 8192};
                default:
                    throw std::runtime_error("Invalid TextureResolution level");
            }
        }

        struct Handle {
            StreamTexture::Slot slot{};
            TextureResolution level{};
            Vec2i size; // The original texture size

            bool operator==(const Handle &other) const {
                return slot == other.slot && level == other.level && size == other.size;
            }

            [[nodiscard]] Vec2f getScale() const {
                return size.convert<float>() / getTextureResolutionLevelSize(level).convert<float>();
            }
        };

        explicit TextureStreamer(rg::Heap &heap) {
            for (auto res = RESOLUTION_BEGIN; res <= RESOLUTION_END; res = static_cast<TextureResolution>(res + 1)) {
                rg::Texture desc;
                desc.textureType = rg::TEXTURE_2D;
                desc.size = getTextureResolutionLevelSize(res);
                desc.format = rg::ColorFormat::RGBA8;
                desc.mipLevels = rg::Texture::calculateMipLevels(desc.size);
                desc.capabilities = rg::Texture::CAPABILITY_SAMPLED;
                desc.mipMode = rg::NEAREST;
                desc.filterMin = rg::NEAREST;
                desc.filterMag = rg::NEAREST;
                desc.wrapping = rg::TextureWrapping::CLAMP_TO_BORDER;
                textures.emplace(res, StreamTexture(heap, desc));
            }
        }

        Handle upload(const ImageRGBA &image) {
            const auto level = getClosestMatchingResolutionLevel(image.getResolution());
            auto &texture = textures.at(level);
            const auto slot = texture.create();
            texture.upload(slot, image);
            return {slot, level, image.getResolution()};
        }

        void destroy(const Handle &handle) {
            textures.at(handle.level).destroy(handle.slot);
        }

        bool isUploadComplete(const Handle &handle) {
            return textures.at(handle.level).isUploadComplete(handle.slot);
        }

        void flush(const Handle &handle) {
            textures.at(handle.level).flush(handle.slot);
        }

        std::unordered_map<TextureResolution, rg::HeapResource<rg::Texture> > commit(rg::GraphBuilder &ctx) {
            std::unordered_map<TextureResolution, rg::HeapResource<rg::Texture> > ret;
            for (auto &pair: textures) {
                ret.emplace(pair.first, pair.second.commit(ctx));
            }
            return ret;
        }

    private:
        static TextureResolution getClosestMatchingResolutionLevel(const Vec2i &size) {
            for (auto res = RESOLUTION_BEGIN; res <= RESOLUTION_END; res = static_cast<TextureResolution>(res + 1)) {
                const auto resSize = getTextureResolutionLevelSize(res);
                if (size.x <= resSize.x && size.y <= resSize.y) {
                    return res;
                }
            }
            throw std::runtime_error("No matching resolution level found");
        }

        std::unordered_map<TextureResolution, StreamTexture> textures;
    };
}

#endif //XENGINE_TEXTURESTREAMER_HPP
