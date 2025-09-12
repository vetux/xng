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

#ifndef XENGINE_RENDERGRAPHTEXTUREPROPERTIES_HPP
#define XENGINE_RENDERGRAPHTEXTUREPROPERTIES_HPP

namespace xng {
    enum TextureType : int {
        TEXTURE_2D = 0,
        TEXTURE_2D_MULTISAMPLE,
        TEXTURE_CUBE_MAP
    };

    enum CubeMapFace : int {
        POSITIVE_X = 0,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z
    };

    enum ColorFormat : int {
        //Base Formats
        DEPTH = 0,
        DEPTH_STENCIL,
        R,
        RG,
        RGB,
        RGBA,

        //Compressed Formats
        R_COMPRESSED,
        RG_COMPRESSED,
        RGB_COMPRESSED,
        RGBA_COMPRESSED,

        //Sized normalized float
        R8,
        RG8,
        RGB8,
        RGBA8,

        R16,
        RG16,
        RGB16,
        RGBA16,

        RGB12,
        RGBA12,
        RGB10,

        //Sized float
        R16F,
        RG16F,
        RGB16F,
        RGBA16F,

        R32F,
        RG32F,
        RGB32F,
        RGBA32F,

        //Sized integer
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

        //Sized unsigned integer
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

    enum MipMapFiltering : int {
        NEAREST_MIPMAP_NEAREST = 0,
        LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR
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

    inline MipMapFiltering &operator<<(MipMapFiltering &value, const Message &message) {
        value = (MipMapFiltering) message.asInt();
        return value;
    }

    inline Message &operator>>(const MipMapFiltering &value, Message &message) {
        message = (int) value;
        return message;
    }
}

#endif //XENGINE_RENDERGRAPHTEXTUREPROPERTIES_HPP
