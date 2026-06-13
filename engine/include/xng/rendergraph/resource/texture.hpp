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

#ifndef XENGINE_RENDERGRAPH_TEXTURE_HPP
#define XENGINE_RENDERGRAPH_TEXTURE_HPP

#include <variant>

#include "xng/math/vector2.hpp"
#include "xng/math/vector4.hpp"

#include "xng/rendergraph/textureproperties.hpp"

namespace xng::rg {
    struct Texture {
        struct DepthStencilClearValue {
            float clearDepth{};
            unsigned int clearStencil{};

            DepthStencilClearValue() = default;

            DepthStencilClearValue(const float clearDepth, const unsigned int clearStencil)
                : clearDepth(clearDepth), clearStencil(clearStencil) {
            }
        };

        typedef std::variant<Vector4<uint8_t>, Vec4f, Vec4i, Vec4u, DepthStencilClearValue, float, unsigned> ClearValue;

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
            unsigned int mipLevel = 0;
            long arrayLayer = -1;
            CubeMapFace face = FACE_UNDEFINED;

            SubResource() = default;

            explicit SubResource(const long arrayLayer)
                : arrayLayer(arrayLayer) {
            }

            explicit SubResource(const CubeMapFace face)
                : face(face) {
            }

            SubResource(const unsigned int mipLevel,
                        const long arrayLayer,
                        const CubeMapFace face)
                : mipLevel(mipLevel), arrayLayer(arrayLayer), face(face) {
            }

            bool operator==(const SubResource &other) const {
                return mipLevel == other.mipLevel && arrayLayer == other.arrayLayer && face == other.face;
            }
        };

        Capability capabilities = CAPABILITY_NONE;

        Vec2u size = {1, 1};
        TextureType textureType = TEXTURE_2D;
        ColorFormat format = RGBA8;

        TextureWrapping wrapping = CLAMP_TO_BORDER;
        TextureFiltering filterMin = NEAREST;
        TextureFiltering filterMag = NEAREST;

        int samples = 1; //Ignored if texture is not TEXTURE_2D_MULTISAMPLE
        bool fixedSampleLocations = false;

        unsigned int mipLevels = 1; // The number of mip levels created for this texture
        TextureFiltering mipMode = NEAREST; // The mip map filtering mode (Filtering between mip levels)

        size_t arrayLayers = 0; // The number of layers in the texture if textureType == TEXTURE_*_ARRAY

        std::variant<Vec4f, Vec4i, Vec4u> borderColor = Vec4f(0);

        float maxAnisotropy = 8.0f;

        explicit Texture(const Capability capabilities = CAPABILITY_NONE,
                         Vec2u size = {1, 1},
                         const TextureType texture_type = TEXTURE_2D,
                         const ColorFormat format = RGBA8,
                         const TextureWrapping wrapping = CLAMP_TO_BORDER,
                         const TextureFiltering filter_min = NEAREST,
                         const TextureFiltering filter_mag = NEAREST,
                         const int samples = 1,
                         const bool fixed_sample_locations = false,
                         const unsigned int mip_levels = 1,
                         const TextureFiltering mip_map_filter = NEAREST,
                         const size_t array_layers = 0,
                         std::variant<Vec4f, Vec4i, Vec4u> border_color = Vec4f(0),
                         const float max_anisotropy = 8.0f)
            : capabilities(capabilities),
              size(std::move(size)),
              textureType(texture_type),
              format(format),
              wrapping(wrapping),
              filterMin(filter_min),
              filterMag(filter_mag),
              samples(samples),
              fixedSampleLocations(fixed_sample_locations),
              mipLevels(mip_levels),
              mipMode(mip_map_filter),
              arrayLayers(array_layers),
              borderColor(std::move(border_color)),
              maxAnisotropy(max_anisotropy) {
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
                   && mipMode == other.mipMode
                   && fixedSampleLocations == other.fixedSampleLocations
                   && borderColor == other.borderColor
                   && arrayLayers == other.arrayLayers
                   && capabilities == other.capabilities;
        }

        Vec2u getMipLevelSize(const unsigned int mipLevel) const {
            return getMipLevelSize(size, mipLevel);
        }

        static Vec2u getMipLevelSize(const Vec2u &size, const unsigned int mipLevel) {
            if (mipLevel == 0) {
                return size;
            }
            return {std::max(1u, size.x >> mipLevel), std::max(1u, size.y >> mipLevel)};
        }

        static unsigned int calculateMipLevels(const Vec2u &size) {
            const auto maxDimension = std::max(size.x, size.y);
            return static_cast<int>(std::floor(std::log2(maxDimension))) + 1;
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
            hash_combine(ret, buffer.mipMode);
            hash_combine(ret, buffer.fixedSampleLocations);

            hash_combine(ret, buffer.borderColor.index());
            switch (buffer.borderColor.index()) {
                case 0: {
                    auto &color = std::get<Vec4f>(buffer.borderColor);
                    hash_combine(ret, color.x);
                    hash_combine(ret, color.y);
                    hash_combine(ret, color.z);
                    hash_combine(ret, color.w);
                    break;
                }
                case 1: {
                    auto &color = std::get<Vec4i>(buffer.borderColor);
                    hash_combine(ret, color.x);
                    hash_combine(ret, color.y);
                    hash_combine(ret, color.z);
                    hash_combine(ret, color.w);
                    break;
                }
                case 2: {
                    auto &color = std::get<Vec4u>(buffer.borderColor);
                    hash_combine(ret, color.x);
                    hash_combine(ret, color.y);
                    hash_combine(ret, color.z);
                    hash_combine(ret, color.w);
                    break;
                }
                default:
                    break;
            }

            hash_combine(ret, buffer.arrayLayers);
            hash_combine(ret, buffer.capabilities);
            return ret;
        }
    };
}

#endif //XENGINE_RENDERGRAPH_TEXTURE_HPP
