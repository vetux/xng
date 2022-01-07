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

#include "math/matrix.hpp"

namespace xengine {
    Vector4<float> operator*(const Matrix<float, 4, 4> &lhs, const Vector4<float> &rhs) {
        //TODO Fix matrix multiplication operators
        Vector4<float> ret;
        for (int row = 0; row < lhs.height(); row++) {
            float *out;
            switch (row) {
                case 0:
                    out = &ret.x;
                    break;
                case 1:
                    out = &ret.y;
                    break;
                case 2:
                    out = &ret.z;
                    break;
                case 3:
                    out = &ret.w;
                    break;
                default:
                    assert(false);
            }
            
            for (int column = 0; column < lhs.width(); column++) {
                float in;
                switch (column) {
                    case 0:
                        in = rhs.x;
                        break;
                    case 1:
                        in = rhs.y;
                        break;
                    case 2:
                        in = rhs.z;
                        break;
                    case 3:
                        in = rhs.w;
                        break;
                    default:
                        assert(false);
                }
                *out += lhs.get(column, row) * in;
            }
        }
        return ret;
    }

    Vector4<double> operator*(const Matrix<double, 4, 4> &lhs, const Vector4<double> &rhs) {
        Vector4<double> ret;
        for (int row = 0; row < lhs.height(); row++) {
            double *out;
            switch (row) {
                case 0:
                    out = &ret.x;
                    break;
                case 1:
                    out = &ret.y;
                    break;
                case 2:
                    out = &ret.z;
                    break;
                case 3:
                    out = &ret.w;
                    break;
                default:
                    assert(false);
            }

            for (int column = 0; column < lhs.width(); column++) {
                double in;
                switch (column) {
                    case 0:
                        in = rhs.x;
                        break;
                    case 1:
                        in = rhs.y;
                        break;
                    case 2:
                        in = rhs.z;
                        break;
                    case 3:
                        in = rhs.w;
                        break;
                    default:
                        assert(false);
                }
                *out += lhs.get(column, row) * in;
            }
        }
        return ret;
    }

    Vector3<float> operator*(const Matrix<float, 4, 4> &lhs, const Vector3<float> &rhs) {
        Vec4f vec4(rhs.x, rhs.y, rhs.z, 0);
        auto result = lhs * vec4;
        return {result.x, result.y, result.z};
    }

    Vector3<double> operator*(const Matrix<double, 4, 4> &lhs, const Vector3<double> &rhs) {
        Vec4d vec4(rhs.x, rhs.y, rhs.z, 0);
        auto result = lhs * vec4;
        return {result.x, result.y, result.z};
    }

    Matrix<float, 4, 4> operator*(const Matrix<float, 4, 4> &lhs, const Matrix<float, 4, 4> &rhs) {
        Mat4f ret;
        for (int row = 0; row < lhs.height(); row++) {
            for (int column = 0; column < lhs.width(); column++) {
                Vec4f ro;
                ro.x = lhs.get(0, row);
                ro.y = lhs.get(1, row);
                ro.z = lhs.get(2, row);
                ro.w = lhs.get(3, row);

                Vec4f co;
                co.x = rhs.get(column, 0);
                co.y = rhs.get(column, 1);
                co.z = rhs.get(column, 2);
                co.w = rhs.get(column, 3);

                ret.set(column, row, (ro.x * co.x) + (ro.y * co.y) + (ro.z * co.z) + (ro.w * co.w));
            }
        }
        return ret;
    }
}