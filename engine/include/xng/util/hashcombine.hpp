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

#ifndef XENGINE_HASHCOMBINE_HPP
#define XENGINE_HASHCOMBINE_HPP

#include <functional>

namespace xng {
    //https://stackoverflow.com/a/2595226
    XENGINE_EXPORT void hash_combine(std::size_t &seed, std::size_t v);

    template<typename T>
    void hash_combine(std::size_t &seed, const T &v) {
        std::hash<T> hash;
        return hash_combine(seed, hash(v));
    }
}
#endif //XENGINE_HASHCOMBINE_HPP
