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

#ifndef XENGINE_DOWNCAST_HPP
#define XENGINE_DOWNCAST_HPP

namespace xng {
    /**
     * When down casting, it must be ensured that the types are compatible with e.g. custom typenames.
     *
     * Will be replaced with a static_cast to avoid RTTI
     *
     * @tparam T
     * @tparam BASE
     * @param base
     * @return
     */
    template <typename T, typename BASE>
    T down_cast(BASE &base) {
        return dynamic_cast<T>(base);
    }
}

#endif //XENGINE_DOWNCAST_HPP