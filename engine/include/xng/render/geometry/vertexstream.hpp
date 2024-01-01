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

#ifndef XENGINE_VERTEXSTREAM_HPP
#define XENGINE_VERTEXSTREAM_HPP

#include "xng/render/geometry/vertexbuilder.hpp"

namespace xng {
    class VertexStream {
    public:
        VertexStream &addVertex(const Vertex &vertex) {
            vertexBuffer.insert(vertexBuffer.end(), vertex.buffer.begin(), vertex.buffer.end());
            return *this;
        }

        VertexStream &addVertices(const std::vector<Vertex> &value) {
            for (auto &v: value) {
                addVertex(v);
            }
            return *this;
        }

        const std::vector<uint8_t> &getVertexBuffer() const { return vertexBuffer; }

    private:
        std::vector<uint8_t> vertexBuffer;
    };
}

#endif //XENGINE_VERTEXSTREAM_HPP
