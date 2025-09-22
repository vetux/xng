/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_GRAPHICSMATH_HPP
#define XENGINE_GRAPHICSMATH_HPP

#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"
#include "matrix.hpp"

namespace xng::MatrixMath {
    XENGINE_EXPORT Mat4f identity();

    XENGINE_EXPORT Mat4f inverse(const Mat4f &mat);

    XENGINE_EXPORT Mat4f transpose(const Mat4f &mat);

    /**
     * Returns a matrix which can be used for multiplying with a column vector.
     *
     * The returned matrix should be multiplied with a column vector or transposed to multiply with a row vector.
     *
     * @param fovy
     * @param aspect
     * @param zNear
     * @param zFar
     * @return
     */
    XENGINE_EXPORT Mat4f perspective(float fovy, float aspect, float zNear, float zFar);

    /**
     * Returns a matrix which can be used for multiplying with a column vector.
     *
     * The returned matrix should be multiplied with a column vector or transposed to multiply with a row vector.
     *
     * @param left
     * @param right
     * @param bottom
     * @param top
     * @param zNear
     * @param zFar
     * @return
     */
    XENGINE_EXPORT Mat4f ortho(float left, float right, float bottom, float top, float zNear, float zFar);

    XENGINE_EXPORT Vec3f unProject(Vec2i windowCoordinates,
                                   Mat4f viewMatrix,
                                   Mat4f projectionMatrix,
                                   Vec2i viewportOffset,
                                   Vec2i viewportSize);

    /**
     * The returned matrix should be multiplied with a column vector or transposed to multiply with a row vector.
     *
     * @param translationValue
     * @return
     */
    XENGINE_EXPORT  Mat4f translate(const Vec3f &translationValue);

    /**
     * The returned matrix should be multiplied with a column vector or transposed to multiply with a row vector.
     *
     * @param scaleValue
     * @return
     */
    XENGINE_EXPORT Mat4f scale(const Vec3f &scaleValue);

    /**
     * The returned matrix should be multiplied with a column vector or transposed to multiply with a row vector.
     *
     * @param rotationValue
     * @return
     */
    XENGINE_EXPORT Mat4f rotate(const Vec3f &rotationValue);

    XENGINE_EXPORT Mat4f lookAt(const Vec3f &eye, const Vec3f &center , const Vec3f &up);
}

#endif //XENGINE_GRAPHICSMATH_HPP
