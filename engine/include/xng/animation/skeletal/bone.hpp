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

#ifndef XENGINE_BONE_HPP
#define XENGINE_BONE_HPP

#include "xng/math/matrix.hpp"

namespace xng {
    struct VertexWeight {
        size_t vertex; // The index of the vertex in Mesh::vertices
        float weight;
    };
    struct XENGINE_EXPORT Bone {
        std::string name; // The name of the bone
        Mat4f offset; // The offset of the bone
        std::vector<VertexWeight> weights;
    };
}

#endif //XENGINE_BONE_HPP