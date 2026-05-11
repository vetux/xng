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

#ifndef XENGINE_DRAWCALL_HPP
#define XENGINE_DRAWCALL_HPP

#include <cstddef>

namespace xng::rg {
    /**
     * A DrawCall specifies which portion of the bound index or vertex buffer to draw.
     */
    struct DrawCall {
        explicit DrawCall(const size_t offset = 0, const size_t count = 0)
            : offset(offset), count(count) {
        }

        // The index of the first vertex or the byte offset of the first index to read from the bound vertex / index buffer
        size_t offset = 0;

        // The number of indices or vertices to draw.
        size_t count = 0;
    };
}

#endif //XENGINE_DRAWCALL_HPP
