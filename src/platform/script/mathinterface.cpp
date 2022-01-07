/**
 *  XEngine - C++ game engine library
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

#include "mathinterface.hpp"

#include "math/matrix.hpp"
#include "math/matrixmath.hpp"

namespace xengine {
    namespace MathInterface {
        MarshalMatrix convert(const Mat4f mat) {
            MarshalMatrix ret{};

            ret.x0 = mat.get(0, 0);
            ret.y0 = mat.get(1, 0);
            ret.z0 = mat.get(2, 0);
            ret.w0 = mat.get(3, 0);

            ret.x1 = mat.get(0, 1);
            ret.y1 = mat.get(1, 1);
            ret.z1 = mat.get(2, 1);
            ret.w1 = mat.get(3, 1);

            ret.x2 = mat.get(0, 2);
            ret.y2 = mat.get(1, 2);
            ret.z2 = mat.get(2, 2);
            ret.w2 = mat.get(3, 2);

            ret.x3 = mat.get(0, 3);
            ret.y3 = mat.get(1, 3);
            ret.z3 = mat.get(2, 3);
            ret.w3 = mat.get(3, 3);

            return ret;
        }

        Mat4f convert(const MarshalMatrix &mat) {
            Mat4f ret;

            ret.set(0, 0, mat.x0);
            ret.set(1, 0, mat.y0);
            ret.set(2, 0, mat.z0);
            ret.set(3, 0, mat.w0);

            ret.set(0, 1, mat.x1);
            ret.set(1, 1, mat.y1);
            ret.set(2, 1, mat.z1);
            ret.set(3, 1, mat.w1);

            ret.set(0, 2, mat.x2);
            ret.set(1, 2, mat.y2);
            ret.set(2, 2, mat.z2);
            ret.set(3, 2, mat.w2);

            ret.set(0, 3, mat.x3);
            ret.set(1, 3, mat.y3);
            ret.set(2, 3, mat.z3);
            ret.set(3, 3, mat.w3);

            return ret;
        }

        MarshalMatrix perspectiveMatrix(float fovy, float aspectRatio, float zNear, float zFar) {
            return convert(MatrixMath::perspective(fovy, aspectRatio, zNear, zFar));
        }

        MarshalMatrix orthographicMatrix(float left, float right, float bottom, float top, float zNear, float zFar) {
            return convert(MatrixMath::ortho(left, right, bottom, top, zNear, zFar));
        }

        MarshalMatrix inverse(MarshalMatrix mat) {
            return convert(MatrixMath::inverse(convert(mat)));
        }

        MarshalMatrix transpose(MarshalMatrix mat) {
            return convert(MatrixMath::transpose(convert(mat)));
        }

        MarshalMatrix translate(float x, float y, float z) {
            return convert(MatrixMath::translate({x, y, z}));
        }

        MarshalMatrix scale(float x, float y, float z) {
            return convert(MatrixMath::scale({x, y, z}));
        }

        MarshalMatrix rotate(float x, float y, float z) {
            return convert(MatrixMath::rotate({x, y, z}));
        }
    }
}