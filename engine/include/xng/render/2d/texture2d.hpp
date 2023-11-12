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

#ifndef XENGINE_TEXTURE2D_HPP
#define XENGINE_TEXTURE2D_HPP

#include <utility>

#include "xng/render/atlas/textureatlashandle.hpp"

namespace xng {
    class Texture2D {
    public:
        class Deallocator {
        public:
            explicit Deallocator(std::function<void()> callback) : callback(std::move(callback)) {}

            ~Deallocator() {
                callback();
            }

            std::function<void()> callback;
        };

        Texture2D() = default;

        Texture2D(std::shared_ptr<Deallocator> dealloc, TextureAtlasHandle handle)
                : dealloc(std::move(dealloc)),
                  handle(std::move(handle)) {}

        const TextureAtlasHandle &getHandle() const {
            return handle;
        }

        const Vec2i &getSize() const {
            return handle.size;
        }

    private:
        std::shared_ptr<Deallocator> dealloc;
        TextureAtlasHandle handle;
    };
}
#endif //XENGINE_TEXTURE2D_HPP
