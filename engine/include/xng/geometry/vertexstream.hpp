/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_VERTEXSTREAM_HPP
#define XENGINE_VERTEXSTREAM_HPP

#include "xng/geometry/vertexbuilder.hpp"

namespace xng {
    class VertexStream {
    public:
        VertexStream &addVertex(const Vertex &vertex) {
            size_t bytes = vertex.buffer.size() * sizeof(float);
            vertexBuffer.resize(vertexBuffer.size() + bytes);
            std::copy(vertex.buffer.begin(), vertex.buffer.end(), vertexBuffer.end() - (long) bytes);
            return *this;
        }

        const std::vector<uint8_t> &getVertexBuffer() const { return vertexBuffer; }

    private:
        std::vector<Vertex> vertices;
        std::vector<uint8_t> vertexBuffer;
    };
}

#endif //XENGINE_VERTEXSTREAM_HPP
