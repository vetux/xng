/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_DRAWCALL_HPP
#define XENGINE_DRAWCALL_HPP

namespace xng {
    enum IndexType {
        UNSIGNED_INT
    };

    inline size_t getIndexTypeSize(IndexType type) {
        switch (type) {
            default:
                return sizeof(unsigned int);
        }
    }

    /**
     * A DrawCall specifies which portion of the bound index or vertex buffer to draw.
     */
    struct DrawCall {
        explicit DrawCall(size_t offset = 0, size_t count = 0, IndexType indexType = UNSIGNED_INT)
                : offset(offset),
                  count(count),
                  indexType(
                          indexType) {}

        size_t offset = 0; // The offset into the index or vertex buffer at which to begin reading indices or vertices in BYTES
        size_t count = 0; // The number of indices or vertices to draw.
        IndexType indexType = UNSIGNED_INT; // The type of the indices, ignored when not indexing
    };
}

#endif //XENGINE_DRAWCALL_HPP
