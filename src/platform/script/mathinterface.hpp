/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_MATHINTERFACE_HPP
#define MANA_MATHINTERFACE_HPP

namespace engine {
    namespace MathInterface {
        extern "C"
        {
        struct MarshalMatrix {
            float x0, y0, z0, w0;
            float x1, y1, z1, w1;
            float x2, y2, z2, w2;
            float x3, y3, z3, w3;
        };

        MarshalMatrix perspectiveMatrix(float fovy, float aspectRatio, float zNear, float zFar);

        MarshalMatrix orthographicMatrix(float left,
                                         float right,
                                         float bottom,
                                         float top,
                                         float zNear,
                                         float zFar);

        MarshalMatrix inverse(MarshalMatrix mat);

        MarshalMatrix transpose(MarshalMatrix mat);

        MarshalMatrix translate(float x, float y, float z);

        MarshalMatrix scale(float x, float y, float z);

        MarshalMatrix rotate(float x, float y, float z);
        }
    }
}

#endif //MANA_MATHINTERFACE_HPP
