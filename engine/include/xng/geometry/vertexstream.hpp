/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_VERTEXSTREAM_HPP
#define XENGINE_VERTEXSTREAM_HPP

#include "xng/geometry/vertexbuilder.hpp"

namespace xng {
    class VertexStream {
    public:
        VertexStream &addVertex(const Vertex &vertex) {
            size_t bytes = vertex.buffer.size() * sizeof(float);
            std::vector<uint8_t> vertexBytes;
            vertexBytes.resize(bytes);
            for (auto i = 0; i < vertex.buffer.size(); i++) {
                reinterpret_cast<float &>(vertexBytes.at(i * sizeof(float))) = vertex.buffer.at(i);
            }
            auto start = vertexBuffer.size();
            vertexBuffer.resize(start + bytes);
            std::copy(vertexBytes.begin(), vertexBytes.end(), vertexBuffer.begin() + static_cast<long>(start));

            vertices.emplace_back(vertex);
            return *this;
        }

        VertexStream &addVertices(const std::vector<Vertex> &vertices) {
            for (auto &v: vertices) {
                addVertex(v);
            }
            return *this;
        }

        const std::vector<Vertex> &getVertices() const { return vertices; }

        const std::vector<uint8_t> &getVertexBuffer() const { return vertexBuffer; }

    private:
        std::vector<Vertex> vertices;
        std::vector<uint8_t> vertexBuffer;
    };
}

#endif //XENGINE_VERTEXSTREAM_HPP
