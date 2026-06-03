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

#ifndef XENGINE_RENDERGRAPH_TEXTUREPROPERTIES_HPP
#define XENGINE_RENDERGRAPH_TEXTUREPROPERTIES_HPP

#include "xng/io/message.hpp"

namespace xng::rg {
    enum TextureType : int {
        TEXTURE_2D = 0,
        TEXTURE_2D_MULTISAMPLE,
        TEXTURE_CUBE_MAP,
        TEXTURE_2D_ARRAY,
        TEXTURE_2D_MULTISAMPLE_ARRAY,
        TEXTURE_CUBE_MAP_ARRAY,
    };

    enum CubeMapFace : int {
        FACE_UNDEFINED = -1,
        POSITIVE_X = 0,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z
    };

    /**
     * 3 Component color formats are only valid formats for buffers
     * and cannot be used on textures.
     */
    enum ColorFormat : int {
        // Normalized Color Format (0-255 unsigned integer stored -> sampled as normalized float 0.0-1.0)
        R8 = 0,
        RG8,
        RGB8,
        RGBA8,

        R16,
        RG16,
        RGB16,
        RGBA16,

        // Normalized sRGB Color Format (0-255 unsigned integer in sRGB space -> sampled as normalized float 0.0-1.0)
        SRGB8,
        SRGB8_ALPHA8, // rgb = 8bit sRGB, alpha = 8bit normalized unsigned integer

        // Float Color Formats (Not Normalized)
        R16F,
        RG16F,
        RGB16F,
        RGBA16F,

        R32F,
        RG32F,
        RGB32F,
        RGBA32F,

        // Signed Integer Color Formats (Not Normalized)
        R8I,
        RG8I,
        RGB8I,
        RGBA8I,

        R16I,
        RG16I,
        RGB16I,
        RGBA16I,

        R32I,
        RG32I,
        RGB32I,
        RGBA32I,

        // Unsigned Integer Color Formats (Not Normalized)
        R8UI,
        RG8UI,
        RGB8UI,
        RGBA8UI,

        R16UI,
        RG16UI,
        RGB16UI,
        RGBA16UI,

        R32UI,
        RG32UI,
        RGB32UI,
        RGBA32UI,

        // Normalized Depth Format (16bit unsigned integer)
        DEPTH_16,

        // Depth Format (32bit IEEE Float)
        DEPTH_32F,

        // Stencil Format (8bit unsigned integer)
        STENCIL_8,

        // Combined depth / stencil format
        DEPTH24_STENCIL8, // depth: 24bit unsigned integer (normalized), stencil: 8bit unsigned integer
        DEPTH32F_STENCIL8, // depth: 32bit IEEE float, stencil: 8bit unsigned integer

        // Compressed Formats, Support must be queried through Runtime::getSupportedFormats
        // Compressed Textures are immutable.
        // Users must supply already compressed data in the matching format in TransferContext::copyBufferToTexture

        // Block Compression, Mostly supported on desktop not on mobile.

        // Modern Block Compression
        RGBA_BC7,
        RGBA_BC7_SRGB,

        RGB_BC6H_SFLOAT,
        RGB_BC6H_UFLOAT,

        RG_BC5_SNORM,
        RG_BC5_UNORM,

        R_BC4_SNORM,
        R_BC4_UNORM,

        // Legacy Block Compression
        RGBA_BC3,
        RGBA_BC3_SRGB,

        RGBA_BC2,
        RGBA_BC2_SRGB,

        RGBA_BC1,
        RGBA_BC1_SRGB,

        RGB_BC1,
        RGB_BC1_SRGB,

        // Adaptive Scalable Texture Compression, Mostly supported on mobile on desktop support is sparse (Software emulation)
        RGBA_ASTC_4x4,
        RGBA_ASTC_6x6,
        RGBA_ASTC_8x8,
        RGBA_ASTC_12x12,

        RGBA_ASTC_4x4_SFLOAT,
        RGBA_ASTC_6x6_SFLOAT,
        RGBA_ASTC_8x8_SFLOAT,
        RGBA_ASTC_12x12_SFLOAT,

        RGBA_ASTC_4x4_SRGB,
        RGBA_ASTC_6x6_SRGB,
        RGBA_ASTC_8x8_SRGB,
        RGBA_ASTC_12x12_SRGB,

        COLOR_FORMAT_BEGIN = R8,
        COLOR_FORMAT_END = RGBA_ASTC_12x12_SRGB,
        COLOR_FORMAT_COMPRESSED_START = RGBA_BC7,
    };

    enum TextureWrapping : int {
        REPEAT = 0,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum TextureFiltering : int {
        NEAREST = 0,
        LINEAR
    };

    inline TextureType &operator<<(TextureType &value, const Message &message) {
        value = (TextureType) message.asInt();
        return value;
    }

    inline Message &operator>>(const TextureType &value, Message &message) {
        message = (int) value;
        return message;
    }

    inline CubeMapFace &operator<<(CubeMapFace &value, const Message &message) {
        value = (CubeMapFace) message.asInt();
        return value;
    }

    inline Message &operator>>(const CubeMapFace &value, Message &message) {
        message = (int) value;
        return message;
    }

    inline ColorFormat &operator<<(ColorFormat &value, const Message &message) {
        value = (ColorFormat) message.asInt();
        return value;
    }

    inline Message &operator>>(const ColorFormat &value, Message &message) {
        message = (int) value;
        return message;
    }

    inline TextureWrapping &operator<<(TextureWrapping &value, const Message &message) {
        value = (TextureWrapping) message.asInt();
        return value;
    }

    inline Message &operator>>(const TextureWrapping &value, Message &message) {
        message = (int) value;
        return message;
    }

    inline TextureFiltering &operator<<(TextureFiltering &value, const Message &message) {
        value = (TextureFiltering) message.asInt();
        return value;
    }

    inline Message &operator>>(const TextureFiltering &value, Message &message) {
        message = (int) value;
        return message;
    }
}

#endif //XENGINE_RENDERGRAPH_TEXTUREPROPERTIES_HPP
