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

#ifndef XENGINE_VERTEX_HPP
#define XENGINE_VERTEX_HPP

#include "math/vector3.hpp"

namespace xengine {
    struct XENGINE_EXPORT Vertex {
        float data[14];

        Vertex(Vec3f position, Vec3f normal, Vec2f uv, Vec3f tangent, Vec3f bitangent) : data() {
            data[0] = position.x;
            data[1] = position.y;
            data[2] = position.z;
            data[3] = normal.x;
            data[4] = normal.y;
            data[5] = normal.z;
            data[6] = uv.x;
            data[7] = uv.y;
            data[8] = tangent.x;
            data[9] = tangent.y;
            data[10] = tangent.z;
            data[11] = bitangent.x;
            data[12] = bitangent.y;
            data[13] = bitangent.z;
        }

        Vertex(Vec3f position, Vec2f uv) : Vertex(position, {}, uv, {}, {}) {}

        explicit Vertex(Vec3f position) : Vertex(position, {}, {}, {}, {}) {}

        Vertex() = default;

        Vec3f position() const {
            return {data[0], data[1], data[2]};
        }

        Vec3f normal() const {
            return {data[3], data[4], data[5]};
        }

        Vec2f uv() const {
            return {data[6], data[7]};
        }

        Vec3f tangent() const {
            return {data[8], data[9], data[10]};
        }

        Vec3f bitangent() const {
            return {data[11], data[12], data[13]};
        }
    };
}

#endif //XENGINE_VERTEX_HPP
