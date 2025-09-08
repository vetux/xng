/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_BT3CONVERT_HPP
#define XENGINE_BT3CONVERT_HPP

#include "xng/math/vector3.hpp"

#include "btBulletDynamicsCommon.h"

namespace xng {
    Vec3f convert(btVector3 vec) {
        if (isnan(vec.x())
            || isnan(vec.y())
            || isnan(vec.z()))
            throw std::runtime_error("Nan Value");
        return {vec.x(), vec.y(), vec.z()};
    }

    btVector3 convert(const Vec3f &vec) {
        if (isnan(vec.x)
            || isnan(vec.y)
            || isnan(vec.z))
            throw std::runtime_error("Nan Value");
        return {vec.x, vec.y, vec.z};
    }

    Quaternion convert(btQuaternion quat) {
        if (isnan(quat.x())
            || isnan(quat.y())
            || isnan(quat.z())
            || isnan(quat.w()))
            throw std::runtime_error("Nan Value");
        return {quat.w(), quat.x(), quat.y(), quat.z()};
    }

    btQuaternion convert(const Quaternion &quat) {
        if (isnan(quat.x)
            || isnan(quat.y)
            || isnan(quat.z)
            || isnan(quat.w))
            throw std::runtime_error("Nan Value");
        return {quat.x, quat.y, quat.z, quat.w};
    }
}
#endif //XENGINE_BT3CONVERT_HPP
