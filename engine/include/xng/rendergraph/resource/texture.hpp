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

#ifndef XENGINE_RENDERGRAPH_TEXTURE_HPP
#define XENGINE_RENDERGRAPH_TEXTURE_HPP

#include <variant>

#include "xng/math/vector2.hpp"

#include "xng/rendergraph/color.hpp"
#include "xng/rendergraph/textureproperties.hpp"

namespace xng::rendergraph {
    struct Texture {
        struct DepthStencilClearValue {
            float clearDepth{};
            unsigned int clearStencil{};
        };

        typedef std::variant<ColorRGBA, Vec4f, Vec4i, Vec4u, DepthStencilClearValue> ClearValue;

        /**
         * Capability describes how a texture can be used.
         *
         * Bitmask.
         */
        enum Capability : uint32_t {
            CAPABILITY_NONE = 0,
            CAPABILITY_SAMPLED = 1,
            CAPABILITY_STORAGE = 2,
            CAPABILITY_COLOR_ATTACHMENT = 4,
            CAPABILITY_DEPTH_STENCIL_ATTACHMENT = 8,
            CAPABILITY_TRANSFER_SRC = 16,
            CAPABILITY_TRANSFER_DST = 32,
        };

        /**
         * Each texture has multiple individually addressable sub resources.
         *
         * Each key specifies:
         *  An array layer for TEXTURE_*_ARRAY textures
         *  A face for TEXTURE_CUBE_MAP_* textures
         *  A specific mip level
         *
         * A TEXTURE_2D texture has 1 * mipLevels addressable sub resources.
         * A TEXTURE_CUBE_MAP texture has 6(Each Face) * mipLevels addressable sub resources.
         * A TEXTURE_2D_ARRAY texture has arrayLayers * mipLevels addressable sub resources.
         * A TEXTURE_CUBE_MAP_ARRAY texture has arrayLayers * 6(Each Face) * mipLevels addressable sub resources.
         */
        struct SubResource {
            size_t arrayLayer = 0;
            CubeMapFace face = CubeMapFace::POSITIVE_X;
            size_t mipLevel = 0;

            SubResource() = default;

            explicit SubResource(const size_t arrayLayer)
                : arrayLayer(arrayLayer) {
            }

            explicit SubResource(const CubeMapFace face)
                : face(face) {
            }

            SubResource(const size_t arrayLayer,
                        const CubeMapFace face,
                        const size_t mipLevel)
                : arrayLayer(arrayLayer), face(face), mipLevel(mipLevel) {
            }
        };

        Capability capabilities = CAPABILITY_NONE;

        Vec2i size = {1, 1};
        TextureType textureType = TEXTURE_2D;
        ColorFormat format = RGBA;

        TextureWrapping wrapping = CLAMP_TO_BORDER;
        TextureFiltering filterMin = NEAREST;
        TextureFiltering filterMag = NEAREST;

        int samples = 1; //Ignored if texture is not TEXTURE_2D_MULTISAMPLE
        bool fixedSampleLocations = false;

        int mipLevels = 1; // The number of mip levels created for this texture
        MipMapFiltering mipMapFilter = NEAREST_MIPMAP_LINEAR;

        size_t arrayLayers = 0; // The number of layers in the texture if textureType == TEXTURE_*_ARRAY

        ColorRGBA borderColor = ColorRGBA::black(1, 0);

        static int calculateMipLevels(const Vec2i &size) {
            const auto maxDimension = std::max(size.x, size.y);
            return static_cast<int>(std::floor(std::log2(maxDimension))) + 1;
        }

        bool operator==(const Texture &other) const {
            return size == other.size
                   && textureType == other.textureType
                   && format == other.format
                   && wrapping == other.wrapping
                   && filterMin == other.filterMin
                   && filterMag == other.filterMag
                   && samples == other.samples
                   && mipLevels == other.mipLevels
                   && mipMapFilter == other.mipMapFilter
                   && fixedSampleLocations == other.fixedSampleLocations
                   && borderColor == other.borderColor
                   && arrayLayers == other.arrayLayers
                   && capabilities == other.capabilities;
        }
    };

    inline Texture::Capability operator|(const Texture::Capability a, const Texture::Capability b) {
        return static_cast<Texture::Capability>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    class TextureHash {
    public:
        std::size_t operator()(const Texture &buffer) const {
            size_t ret = 0;
            hash_combine(ret, buffer.size);
            hash_combine(ret, buffer.textureType);
            hash_combine(ret, buffer.format);
            hash_combine(ret, buffer.wrapping);
            hash_combine(ret, buffer.filterMin);
            hash_combine(ret, buffer.filterMag);
            hash_combine(ret, buffer.samples);
            hash_combine(ret, buffer.mipLevels);
            hash_combine(ret, buffer.mipMapFilter);
            hash_combine(ret, buffer.fixedSampleLocations);
            hash_combine(ret, buffer.borderColor.r());
            hash_combine(ret, buffer.borderColor.g());
            hash_combine(ret, buffer.borderColor.b());
            hash_combine(ret, buffer.borderColor.a());
            hash_combine(ret, buffer.arrayLayers);
            hash_combine(ret, buffer.capabilities);
            return ret;
        }
    };
}

#endif //XENGINE_RENDERGRAPH_TEXTURE_HPP
