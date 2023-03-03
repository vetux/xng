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

#ifndef XENGINE_VERTEX_HPP
#define XENGINE_VERTEX_HPP

#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"

namespace xng {
    struct XENGINE_EXPORT Vertex {
        std::vector<float> buffer; // TODO: Store vertex data as uint8_t to allow SINGLE Vertex attribute types

        Vertex() = default;
        /* Vertex(Vec3f position, Vec3f normal, Vec2f uv, Vec3f tangent, Vec3f bitangent, Vec4i boneIds, Vec4f boneWeights)
                 : data() {
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
             // Hack: Cast data values to int references for id assignment which should work as long as sizeof(float) == sizeof(int)
             static_assert(sizeof(float) == sizeof(int));
             reinterpret_cast<int &>(data[14]) = boneIds.x;
             reinterpret_cast<int &>(data[15]) = boneIds.y;
             reinterpret_cast<int &>(data[16]) = boneIds.z;
             reinterpret_cast<int &>(data[17]) = boneIds.w;
             data[18] = boneWeights.x;
             data[19] = boneWeights.y;
             data[20] = boneWeights.z;
             data[21] = boneWeights.w;
         }
         */
    };
}

#endif //XENGINE_VERTEX_HPP
