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

#ifndef XENGINE_SHADERTEXTURE_HPP
#define XENGINE_SHADERTEXTURE_HPP

#include "xng/rendergraph/rendergraphtextureproperties.hpp"

namespace xng {
    struct ShaderTexture {
        TextureType type = TEXTURE_2D;
        ColorFormat format = RGBA;

        bool operator==(const ShaderTexture& other) const {
            return type == other.type && format == other.format;
        }

        bool operator!=(const ShaderTexture& other) const {
            return !(*this == other);
        }

        ShaderTexture() = default;

        ShaderTexture(const TextureType type, const ColorFormat format)
            : type(type), format(format) {
        }
    };
}
#endif //XENGINE_SHADERTEXTURE_HPP
