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

#ifndef XENGINE_VERTEXBUFFERDESC_HPP
#define XENGINE_VERTEXBUFFERDESC_HPP

#include <vector>

#include "graphics/vertexattribute.hpp"

#include "algo/hashcombine.hpp"
#include "algo/crc.hpp"

#include "asset/mesh.hpp"

namespace xng {
    struct VertexBufferDesc {
        Mesh::Primitive primitive = Mesh::TRI;
        std::vector<VertexAttribute> vertexLayout{}; // The layout of one vertex
        std::vector<VertexAttribute> instanceLayout{}; // the layout of one instance
        size_t numberOfVertices = 0;
        size_t numberOfInstances = 0;
        size_t numberOfIndices = 0;

        bool operator==(const VertexBufferDesc &other) const {
            return vertexLayout == other.vertexLayout
                   && instanceLayout == other.instanceLayout
                   && numberOfVertices == other.numberOfVertices
                   && numberOfInstances == other.numberOfInstances;
        }
    };
}

using namespace xng;
namespace std {
    template<>
    struct hash<VertexBufferDesc> {
        std::size_t operator()(const VertexBufferDesc &k) const {
            size_t ret = 0;
            for (auto v: k.vertexLayout) {
                hash_combine(ret, v.type);
                hash_combine(ret, v.component);
            }
            for (auto v: k.instanceLayout) {
                hash_combine(ret, v.type);
                hash_combine(ret, v.component);
            }
            hash_combine(ret, k.numberOfVertices);
            hash_combine(ret, k.numberOfInstances);
            return ret;
        }
    };
}

#endif //XENGINE_VERTEXBUFFERDESC_HPP
