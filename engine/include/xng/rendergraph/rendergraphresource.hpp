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

#ifndef XENGINE_RENDERGRAPHRESOURCE_HPP
#define XENGINE_RENDERGRAPHRESOURCE_HPP

#include "xng/util/hashcombine.hpp"

namespace xng {
    struct RenderGraphResource {
        int handle = -1;

        RenderGraphResource() = default;

        explicit RenderGraphResource(const int handle) : handle(handle) {
        }

        explicit operator bool() const {
            return handle >= 0;
        }

        explicit operator int() const {
            return handle;
        }

        bool operator==(const RenderGraphResource &other) const {
            return handle == other.handle;
        }

        bool operator!=(const RenderGraphResource &other) const {
            return !(*this == other);
        }
    };

    class RenderGraphResourceHash {
    public:
        std::size_t operator()(const RenderGraphResource &k) const {
            size_t ret = 0;
            hash_combine(ret, k.handle);
            return ret;
        }
    };
}

#endif //XENGINE_RENDERGRAPHRESOURCE_HPP
