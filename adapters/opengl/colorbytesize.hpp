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

#ifndef XENGINE_COLORBYTESIZE_HPP
#define XENGINE_COLORBYTESIZE_HPP

#include "xng/rendergraph/rendergraphtextureproperties.hpp"

using namespace xng ;

static size_t getColorByteSize(const ColorFormat colorFormat) {
    switch (colorFormat) {
        case R:
            return 1;
        case RG:
            return 2;
        case RGB:
            return 3;
        case RGBA:
            return 4;
        case DEPTH:
            return 4;
        case STENCIL:
            return 1;
        case DEPTH_STENCIL:
            return 4;
        case DEPTH_32F:
            return 4;
        case DEPTH_24:
            return 4;
        case DEPTH_16:
            return 2;
        case STENCIL_32:
            return 4;
        case STENCIL_16:
            return 2;
        case STENCIL_8:
            return 1;
        case DEPTH24_STENCIL8:
            return 4;
        case R_COMPRESSED:
            return 1;
        case RG_COMPRESSED:
            return 2;
        case RGB_COMPRESSED:
            return 3;
        case RGBA_COMPRESSED:
            return 4;
        case R8:
            return 1;
        case RG8:
            return 2;
        case RGB8:
            return 3;
        case RGBA8:
            return 4;
        case R16:
            return 2;
        case RG16:
            return 4;
        case RGB16:
            return 6;
        case RGBA16:
            return 8;
        case RGB12:
            return 6;
        case RGBA12:
            return 8;
        case RGB10:
            return 6;
        case R16F:
            return 2;
        case RG16F:
            return 4;
        case RGB16F:
            return 6;
        case RGBA16F:
            return 8;
        case R32F:
            return 4;
        case RG32F:
            return 8;
        case RGB32F:
            return 12;
        case RGBA32F:
            return 16;
        case R8I:
            return 1;
        case RG8I:
            return 2;
        case RGB8I:
            return 3;
        case RGBA8I:
            return 4;
        case R16I:
            return 2;
        case RG16I:
            return 4;
        case RGB16I:
            return 6;
        case RGBA16I:
            return 8;
        case R32I:
            return 4;
        case RG32I:
            return 8;
        case RGB32I:
            return 12;
        case RGBA32I:
            return 16;
        case R8UI:
            return 1;
        case RG8UI:
            return 2;
        case RGB8UI:
            return 3;
        case RGBA8UI:
            return 4;
        case R16UI:
            return 2;
        case RG16UI:
            return 4;
        case RGB16UI:
            return 6;
        case RGBA16UI:
            return 8;
        case R32UI:
            return 4;
        case RG32UI:
            return 8;
        case RGB32UI:
            return 12;
        case RGBA32UI:
            return 16;
        default:
            throw std::runtime_error("Invalid color format");
    }
}

#endif //XENGINE_COLORBYTESIZE_HPP
