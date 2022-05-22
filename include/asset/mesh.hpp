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

#ifndef XENGINE_MESH_HPP
#define XENGINE_MESH_HPP

#include <vector>
#include <typeindex>

#include "math/vector3.hpp"
#include "math/vector2.hpp"

#include "asset/vertex.hpp"
#include "resource/resource.hpp"

namespace xengine {
    struct XENGINE_EXPORT Mesh : public Resource {
        enum Primitive {
            POINT = 1,
            LINE = 2,
            TRI = 3,
            QUAD = 4
        };

        /**
         * A quad mesh which covers the viewport in normalized screen coordinates
         * @return
         */
        static const Mesh &normalizedQuad();

        static const Mesh &normalizedCube();

        ~Mesh() override = default;

        Resource *clone() override {
            return new Mesh(*this);
        }


        std::type_index getTypeIndex() override;

        bool indexed = false;
        Primitive primitive = POINT;
        std::vector<Vertex> vertices;
        std::vector<uint> indices;

        size_t polyCount() const {
            if (indexed)
                return indices.size() / primitive;
            else
                return vertices.size() / primitive;
        }

        Mesh() = default;

        Mesh(Primitive primitive, std::vector<Vertex> vertices) :
                indexed(false), primitive(primitive), vertices(std::move(vertices)), indices() {}

        Mesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<uint> indices) :
                indexed(true), primitive(primitive), vertices(std::move(vertices)), indices(std::move(indices)) {}

        Mesh(bool indexed, Primitive primitive, std::vector<Vertex> vertices, std::vector<uint> indices)
                : indexed(indexed),
                  primitive(primitive),
                  vertices(std::move(vertices)),
                  indices(std::move(indices)) {}
    };
}

#endif //XENGINE_MESH_HPP
