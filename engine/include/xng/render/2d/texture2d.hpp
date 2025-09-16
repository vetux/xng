/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_TEXTURE2D_HPP
#define XENGINE_TEXTURE2D_HPP

#include <utility>
#include <memory>

#include "xng/render/atlas/textureatlashandle.hpp"

namespace xng {
    class Texture2D {
    public:
        typedef size_t Handle;

        Texture2D() = default;

        Texture2D(const Handle handle, Vec2i size)
            : handle(handle), size(std::move(size)) {
        }

        Handle getHandle() const {
            return handle;
        }

        const Vec2i &getSize() const {
            return size;
        }

    private:
        Handle handle{};
        Vec2i size;
    };
}

#endif //XENGINE_TEXTURE2D_HPP
