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

#include "xng/math/quaternion.hpp"

#include "xng/math/rotation.hpp"
#include "xng/math/matrixmath.hpp"

#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>

#include <cmath>
#include <stdexcept>

namespace xng {
    static Vec3f convertToRadian(const Vec3f &degrees) {
        return {degreesToRadians(degrees.x), degreesToRadians(degrees.y), degreesToRadians(degrees.z)};
    }

    static Vec3f convertToDegrees(const Vec3f &radians) {
        return {radiansToDegrees(radians.x), radiansToDegrees(radians.y), radiansToDegrees(radians.z)};
    }

    static void eulerToQuaternion(Quaternion &q, const Vec3f &eulerDegrees) {
        Vec3f eulerRadian = convertToRadian(eulerDegrees);
        glm::quat gQuat(glm::vec3(eulerRadian.x, eulerRadian.y, eulerRadian.z));
        q = Quaternion(gQuat.w, gQuat.x, gQuat.y, gQuat.z);
    }

    static Vec3f quaternionToEuler(const Quaternion &q) {
        glm::quat gQuat(q.w, q.x, q.y, q.z);
        auto gVec = glm::eulerAngles(gQuat);
        return convertToDegrees(Vec3f(isnanf(gVec.x) ? 0 : gVec.x,
                                      isnanf(gVec.y) ? 0 : gVec.y,
                                      isnanf(gVec.z) ? 0 : gVec.z));
    }

    Quaternion::Quaternion()
            : w(1), x(0), y(0), z(0) {}

    Quaternion::Quaternion(float w, float x, float y, float z)
            : w(w), x(x), y(y), z(z) {}

    Quaternion::Quaternion(Vec4f vec)
            : w(vec.w), x(vec.x), y(vec.y), z(vec.z) {}

    Quaternion::Quaternion(const Vec3f &eulerAngles)
            : w(0), x(0), y(0), z(0) {
        eulerToQuaternion(*this, eulerAngles);
    }

    Quaternion::operator Vec4f() const {
        return {x, y, z, w};
    }

    Quaternion Quaternion::operator*(const Quaternion &other) const {
        glm::quat gQuatLhs(w, x, y, z);
        glm::quat gQuatRhs(other.w, other.x, other.y, other.z);
        glm::quat res = gQuatLhs * gQuatRhs;
        return {res.w, res.x, res.y, res.z};
    }

    void Quaternion::setEulerAngles(Vec3f eulerAngles) {
        eulerToQuaternion(*this, eulerAngles);
    }

    Vec3f Quaternion::getEulerAngles() const {
        return quaternionToEuler(*this);
    }

    Mat4f Quaternion::matrix() const {
        glm::quat gQuat(w, x, y, z);
        auto gMat = static_cast<glm::mat4>(gQuat);
        Mat4f ret;
        for (int col = 0; col < 4; col++) {
            for (int row = 0; row < 4; row++) {
                ret.set(col, row, gMat[col][row]);
            }
        }
        return ret;
    }

    float Quaternion::magnitude() const {
        return sqrtf(powf(w, 2) + powf(x, 2) + powf(y, 2) + powf(z, 2));
    }

    Quaternion& Quaternion::normalize() {
        float m = magnitude();
        w /= m;
        x /= m;
        y /= m;
        z /= m;
        return *this;
    }

    Quaternion Quaternion::normalize(const Quaternion &q) {
        Quaternion ret(q);
        ret.normalize();
        return ret;
    }

    Quaternion Quaternion::slerp(const Quaternion &a, const Quaternion &b, float advance) {
        glm::quat qa(a.w, a.x, a.y, a.z);
        glm::quat qb(b.w, b.x, b.y, b.z);
        auto ret = glm::slerp(qa, qb, advance);
        return {ret.w, ret.x, ret.y, ret.z};
    }

    Messageable &Quaternion::operator<<(const Message &message) {
        message.value("w", w);
        message.value("x", x);
        message.value("y", y);
        message.value("z", z);
        return *this;
    }

    Message &Quaternion::operator>>(Message &message) const {
        message = Message(Message::DICTIONARY);
        message["w"] = w;
        message["x"] = x;
        message["y"] = y;
        message["z"] = z;
        return message;
    }
}