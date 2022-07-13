/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_SHADERBUFFERDESC_HPP
#define XENGINE_SHADERBUFFERDESC_HPP

#include <cstddef>

namespace xng {
    struct ShaderBufferDesc {
        size_t size;

        bool operator==(const ShaderBufferDesc &other) const {
            return size == other.size;
        }
    };
}

using namespace xng;
namespace std {
    template<>
    struct hash<ShaderBufferDesc> {
        std::size_t operator()(const ShaderBufferDesc &k) const {
            size_t ret = 0;
            hash_combine(ret, k.size);
            return ret;
        }
    };
}

#endif //XENGINE_SHADERBUFFERDESC_HPP
