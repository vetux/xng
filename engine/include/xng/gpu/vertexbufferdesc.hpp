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

#ifndef XENGINE_VERTEXBUFFERDESC_HPP
#define XENGINE_VERTEXBUFFERDESC_HPP

#include <vector>

#include "xng/gpu/vertexattribute.hpp"
#include "xng/gpu/renderbuffertype.hpp"

#include "xng/util/hashcombine.hpp"
#include "xng/util/crc.hpp"

#include "xng/geometry/primitive.hpp"

namespace xng {
    struct VertexBufferDesc {
        RenderBufferType bufferType = HOST_VISIBLE;
        size_t size = 0;

        bool operator==(const VertexBufferDesc &other) const {
            return bufferType == other.bufferType
                   && size == other.size;
        }
    };
}

namespace std {
    template<>
    struct hash<xng::VertexBufferDesc> {
        std::size_t operator()(const xng::VertexBufferDesc &k) const {
            size_t ret = 0;

            xng::hash_combine(ret, k.bufferType);
            xng::hash_combine(ret, k.size);

            return ret;
        }
    };
}

#endif //XENGINE_VERTEXBUFFERDESC_HPP
