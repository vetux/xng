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

#ifndef XENGINE_RENDERGRAPH_SHADERTEXTURE_HPP
#define XENGINE_RENDERGRAPH_SHADERTEXTURE_HPP

#include "xng/rendergraph/textureproperties.hpp"

namespace xng::rg {
    struct ShaderTexture {
        TextureType type = TEXTURE_2D;
        ColorFormat format = RGBA8;

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

#endif //XENGINE_RENDERGRAPH_SHADERTEXTURE_HPP
