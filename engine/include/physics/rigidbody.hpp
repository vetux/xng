/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_RIGIDBODY_HPP
#define XENGINE_RIGIDBODY_HPP

#include <set>
#include <memory>

#include "physics/collider.hpp"
#include "collidershape.hpp"

namespace xng {
    class World;

    class XENGINE_EXPORT RigidBody {
    public:
        enum XENGINE_EXPORT RigidBodyType {
            STATIC,
            KINEMATIC,
            DYNAMIC
        };

        virtual ~RigidBody() = default;

        virtual void setRigidBodyType(RigidBodyType type) = 0;

        virtual RigidBodyType getRigidBodyType() = 0;

        virtual void setPosition(const Vec3f &position) = 0;

        virtual Vec3f getPosition() = 0;

        virtual void setVelocity(const Vec3f &velocity) = 0;

        virtual Vec3f getVelocity() = 0;

        virtual void setRotation(const Vec3f &rotation) = 0;

        virtual Vec3f getRotation() = 0;

        virtual void setAngularVelocity(const Vec3f &angularVelocity) = 0;

        virtual Vec3f getAngularVelocity() = 0;

        virtual std::unique_ptr<Collider> createCollider(const ColliderShape &shape,
                                                         float friction,
                                                         float restitution,
                                                         float density,
                                                         bool isSensor) = 0;
    };
}
#endif //XENGINE_RIGIDBODY_HPP
