/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "collider.hpp"
#include "collidershape.hpp"
#include "colliderdesc.hpp"

namespace xng {
    class World;

    class XENGINE_EXPORT RigidBody {
    public:
        enum XENGINE_EXPORT RigidBodyType : int {
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

        virtual void applyForce(const Vec3f &force, const Vec3f &point) = 0;

        virtual void applyTorque(const Vec3f &torque) = 0;

        virtual void applyLinearImpulse(const Vec3f &impulse, const Vec3f &point) = 0;

        virtual void applyAngularImpulse(const Vec3f &impulse) = 0;

        virtual void setLockedRotationAxes(const Vec3b &axes) = 0;

        virtual std::unique_ptr<Collider> createCollider(const ColliderDesc &desc) = 0;

        virtual void setMass(float mass, const Vec3f &center, const Vec3f &rotationalInertia) = 0;

        virtual float getMass() = 0;

        virtual void setGravityScale(float scale) = 0;
    };
}
#endif //XENGINE_RIGIDBODY_HPP
