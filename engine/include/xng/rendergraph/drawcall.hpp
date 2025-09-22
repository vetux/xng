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

#ifndef XENGINE_DRAWCALL_HPP
#define XENGINE_DRAWCALL_HPP

#include <cstddef>

namespace xng {
    enum IndexFormat {
        UNSIGNED_SHORT,
        UNSIGNED_INT,
    };

    /**
     * A DrawCall specifies which portion of the bound index or vertex buffer to draw.
     */
    struct DrawCall {
        explicit DrawCall(size_t offset = 0, size_t count = 0, IndexFormat indexFormat = UNSIGNED_INT)
            : offset(offset),
              count(count),
              indexFormat(indexFormat) {
        }

        // The offset into the index or vertex buffer at which to begin reading indices or vertices in BYTES
        size_t offset = 0;

        size_t count = 0; // The number of indices or vertices to draw.
        IndexFormat indexFormat = UNSIGNED_INT;
    };
}

#endif //XENGINE_DRAWCALL_HPP
