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

#include "math/matrixmath.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//TODO: Remove glm dependency from matrixmath.cpp
namespace xng {
    glm::mat4 convert(const Mat4f &mat) {
        glm::mat4 ret;
        for (int x = 0; x < mat.width(); x++) {
            for (int y = 0; y < mat.height(); y++) {
                ret[x][y] = mat.get(x, y);
            }
        }
        return ret;
    }

    Mat4f convert(const glm::mat4 &mat) {
        Mat4f ret;
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                ret.set(x, y, mat[x][y]);
            }
        }
        return ret;
    }

    Mat4f MatrixMath::identity() {
        Mat4f ret;
        ret.set(0, 0, 1);
        ret.set(1, 1, 1);
        ret.set(2, 2, 1);
        ret.set(3, 3, 1);
        return ret;
    }

    Mat4f MatrixMath::inverse(const Mat4f &mat) {
        return convert(glm::inverse(convert(mat)));
    }

    Mat4f MatrixMath::transpose(const Mat4f &mat) {
        Mat4f ret;
        for (int r = 0; r < mat.height(); r++) {
            for (int c = 0; c < mat.width(); c++) {
                ret.set(r, c, mat.get(c, r));
            }
        }
        return ret;
    }

    Mat4f MatrixMath::perspective(float fovy, float aspect, float zNear, float zFar) {
        return convert(glm::perspective(glm::radians(fovy), aspect, zNear, zFar));
    }

    Mat4f MatrixMath::ortho(float left, float right, float bottom, float top, float zNear,
                            float zFar) {
        return convert(glm::ortho(left, right, bottom, top, zNear, zFar));
    }

    Vec3f MatrixMath::unProject(Vec2i windowCoordinates, Mat4f viewMatrix, Mat4f projectionMatrix, Vec2i viewportOffset,
                                Vec2i viewportSize) {
        auto ret = glm::unProject(glm::vec3(windowCoordinates.x, windowCoordinates.y, 0),
                                  convert(viewMatrix),
                                  convert(projectionMatrix),
                                  glm::vec4(viewportOffset.x, viewportOffset.y, viewportSize.x, viewportSize.y));
        return {ret.x, ret.y, ret.z};
    }

    Mat4f MatrixMath::translate(const Vec3f &translationValue) {
        Mat4f ret = identity();
        ret.set(3, 0, translationValue.x);
        ret.set(3, 1, translationValue.y);
        ret.set(3, 2, translationValue.z);
        return ret;
    }

    Mat4f MatrixMath::scale(const Vec3f &scaleValue) {
        Mat4f ret;
        ret.set(0, 0, scaleValue.x);
        ret.set(1, 1, scaleValue.y);
        ret.set(2, 2, scaleValue.z);
        ret.set(3, 3, 1);
        return ret;
    }

    Mat4f MatrixMath::rotate(const Vec3f &rotationValue) {
        glm::mat4 rot = glm::rotate(glm::mat4(1), glm::radians(rotationValue.x), glm::vec3(1, 0, 0));
        rot = glm::rotate(rot, glm::radians(rotationValue.y), glm::vec3(0, 1, 0));
        rot = glm::rotate(rot, glm::radians(rotationValue.z), glm::vec3(0, 0, 1));
        return convert(rot);
    }
}