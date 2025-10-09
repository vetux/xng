/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_TEXTUREATLASHANDLE_HPP
#define XENGINE_TEXTUREATLASHANDLE_HPP

#include <cstddef>

#include "xng/graphics/3d/textureatlas/textureatlasresolution.hpp"

#include "xng/math/vector2.hpp"

#include "xng/util/hashcombine.hpp"

namespace xng {
    struct TextureAtlasHandle {
        size_t index{};
        TextureAtlasResolution level{};
        Vec2i size; // The original texture size

        bool operator==(const TextureAtlasHandle &other) const{
            return index == other.index && level == other.level && size == other.size;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::TextureAtlasHandle> {
        std::size_t operator()(const xng::TextureAtlasHandle &k) const {
            size_t ret = 0;
            xng::hash_combine(ret, k.index);
            xng::hash_combine(ret, k.level);
            xng::hash_combine(ret, k.size.x);
            xng::hash_combine(ret, k.size.y);
            return ret;
        }
    };
}

#endif //XENGINE_TEXTUREATLASHANDLE_HPP
