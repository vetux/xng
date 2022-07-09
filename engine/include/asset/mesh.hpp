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

#ifndef XENGINE_MESH_HPP
#define XENGINE_MESH_HPP

#include <utility>
#include <vector>
#include <typeindex>

#include "math/vector3.hpp"
#include "math/vector2.hpp"

#include "asset/vertex.hpp"
#include "asset/primitive.hpp"
#include "resource/resource.hpp"
#include "animation/skeletal/rig.hpp"

namespace xng {
    struct XENGINE_EXPORT Mesh : public Resource {
        /**
         * A quad mesh which covers the viewport in normalized screen coordinates
         * @return
         */
        static const Mesh &normalizedQuad();

        static const Mesh &normalizedCube();

        ~Mesh() override = default;

        std::unique_ptr<Resource> clone() override {
            return std::make_unique<Mesh>(*this);
        }

        std::type_index getTypeIndex() override;

        Primitive primitive = POINT;
        std::vector<Vertex> vertices;
        std::vector<uint> indices;

        Rig rig;

        size_t polyCount() const {
            if (indices.empty())
                return vertices.size() / primitive;
            else
                return indices.size() / primitive;
        }

        Mesh() = default;

        Mesh(Primitive primitive, std::vector<Vertex> vertices)
                : primitive(primitive), vertices(std::move(vertices)), indices() {}

        Mesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<uint> indices)
                : primitive(primitive), vertices(std::move(vertices)), indices(std::move(indices)) {}

        Mesh(Primitive primitive, std::vector<Vertex> vertices, std::vector<uint> indices, Rig rig)
                : primitive(primitive),
                  vertices(std::move(vertices)),
                  indices(std::move(indices)),
                  rig(std::move(rig)) {}
    };
}

#endif //XENGINE_MESH_HPP
