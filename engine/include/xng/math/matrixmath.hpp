/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_GRAPHICSMATH_HPP
#define XENGINE_GRAPHICSMATH_HPP

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"
#include "xng/math/vector4.hpp"
#include "xng/math/matrix.hpp"

/**
 * TODO: NDC Independent transform coordinate space
 */
namespace xng::MatrixMath {
    XENGINE_EXPORT Mat4f identity();

    XENGINE_EXPORT Mat4f inverse(const Mat4f &mat);

    XENGINE_EXPORT Mat4f transpose(const Mat4f &mat);

    /**
     * Returns a matrix which can be used for multiplying with a column vector.
     *
     * The returned matrix should be multiplied with a column vector or transposed to multiply with a row vector.
     *
     * The returned matrix applies the projection transformation using left hand coordinate system where
     * +X Is pointing to the right
     * +Y Is pointing upwards
     * +Z Is pointing forwards
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
     * The returned matrix applies the projection transformation using left hand coordinate system where
     * +X Is pointing to the right
     * +Y Is pointing upwards
     * +Z Is pointing forwards
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

    /**
     * Right-handed lookAt, independent of GLM_FORCE_LEFT_HANDED.
     * Use for OpenGL cubemap capture and other cases requiring RH convention.
     */
    XENGINE_EXPORT Mat4f lookAtRH(const Vec3f &eye, const Vec3f &center, const Vec3f &up);

    /**
     * Right-handed symmetric perspective projection with depth range [-1,1].
     * Use for OpenGL cubemap capture and other cases requiring RH convention.
     */
    XENGINE_EXPORT Mat4f perspectiveRH(float fovy, float aspect, float zNear, float zFar);
}

#endif //XENGINE_GRAPHICSMATH_HPP
