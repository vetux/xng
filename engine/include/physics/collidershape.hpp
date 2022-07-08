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

#ifndef XENGINE_COLLIDERSHAPE_HPP
#define XENGINE_COLLIDERSHAPE_HPP

#include "asset/mesh.hpp"

namespace xng {
    struct ColliderShape {
        Primitive primitive = TRI;
        std::vector<Vec3f> vertices;
        std::vector<size_t> indices; // If not empty the indices into vertices in order.

        static ColliderShape fromMesh(const Mesh &mesh) {
            ColliderShape ret;
            ret.primitive = mesh.primitive;
            for (auto &v: mesh.vertices) {
                ret.vertices.emplace_back(v.position());
            }
            for (auto &v: mesh.indices) {
                ret.indices.emplace_back(v);
            }
            return ret;
        }
    };
}

#endif //XENGINE_COLLIDERSHAPE_HPP
