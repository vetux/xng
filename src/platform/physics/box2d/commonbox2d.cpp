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

#ifdef BUILD_ENGINE_PHYSICS_BOX2D

#include "commonbox2d.hpp"

namespace xengine {
    Vec2f convert(const b2Vec2 &vec) {
        return {vec.x, vec.y};
    }

    b2Vec2 convert(const Vec2f &vec) {
        return {vec.x, vec.y};
    }

    RigidBody2D::RigidBodyType convert(b2BodyType type) {
        switch (type) {
            case b2_staticBody:
                return RigidBody2D::STATIC;
            case b2_kinematicBody:
                return RigidBody2D::KINEMATIC;
            case b2_dynamicBody:
                return RigidBody2D::DYNAMIC;
        }
        throw std::runtime_error("");
    }

    b2BodyType convert(RigidBody2D::RigidBodyType type) {
        switch (type) {
            case RigidBody2D::STATIC:
                return b2_staticBody;
            case RigidBody2D::KINEMATIC:
                return b2_kinematicBody;
            case RigidBody2D::DYNAMIC:
                return b2_dynamicBody;
        }
        throw std::runtime_error("");
    }

    b2PolygonShape convert(const std::vector<Vec2f> &points) {
        b2Vec2 bPoints[points.size()];
        for (int i = 0; i < points.size(); i++)
            bPoints[i] = convert(points.at(i));

        b2PolygonShape ret;
        ret.Set(bPoints, points.size());
        return ret;
    }
}

#endif