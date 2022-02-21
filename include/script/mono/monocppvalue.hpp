/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_MONOCPPVALUE_HPP
#define XENGINE_MONOCPPVALUE_HPP

#include <stdexcept>

namespace xengine {
    struct XENGINE_EXPORT MonoCppValue {
        MonoCppValue() = default;

        MonoCppValue(void *ptr) : ptr(ptr) {}

        void *ptr = nullptr;

        template<typename T>
        const T &get() {
            if (ptr == nullptr)
                throw std::runtime_error("null return value");
            return static_cast<const T &>(*static_cast<T *>(ptr));
        }

        template<typename T>
        void setValue(T &p) {
            ptr = p;
        }
    };
}

#endif //XENGINE_MONOCPPVALUE_HPP
