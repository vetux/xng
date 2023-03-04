/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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
#include "renderbuffertype.hpp"

namespace xng {
    struct ShaderBufferDesc {
        RenderBufferType bufferType = HOST_VISIBLE;
        size_t size = 0;

        bool operator==(const ShaderBufferDesc &other) const {
            return size == other.size
                   && bufferType == other.bufferType;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::ShaderBufferDesc> {
        std::size_t operator()(const xng::ShaderBufferDesc &k) const {
            size_t ret = 0;
            xng::hash_combine(ret, k.size);
            xng::hash_combine(ret, k.bufferType);
            return ret;
        }
    };
}

#endif //XENGINE_SHADERBUFFERDESC_HPP
