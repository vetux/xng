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

#include "physics/box2d/commonbox2d.hpp"

namespace xng {
    Vec3f convert(const b2Vec2 &vec) {
        return {vec.x, vec.y, 0};
    }

    b2Vec2 convert(const Vec3f &vec) {
        return {vec.x, vec.y};
    }

    RigidBody::RigidBodyType convert(b2BodyType type) {
        switch (type) {
            case b2_staticBody:
                return RigidBody::STATIC;
            case b2_kinematicBody:
                return RigidBody::KINEMATIC;
            case b2_dynamicBody:
                return RigidBody::DYNAMIC;
        }
        throw std::runtime_error("");
    }

    b2BodyType convert(RigidBody::RigidBodyType type) {
        switch (type) {
            case RigidBody::STATIC:
                return b2_staticBody;
            case RigidBody::KINEMATIC:
                return b2_kinematicBody;
            case RigidBody::DYNAMIC:
                return b2_dynamicBody;
        }
        throw std::runtime_error("");
    }

    b2PolygonShape convert(const std::vector<Vec3f> &points) {
        std::vector<b2Vec2> bPoints(points.size());
        for (int i = 0; i < points.size(); i++)
            bPoints[i] = convert(points.at(i));

        b2PolygonShape ret;
        ret.Set(bPoints.data(), points.size());
        return ret;
    }
}