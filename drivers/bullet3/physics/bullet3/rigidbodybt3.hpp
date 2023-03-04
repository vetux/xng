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

#ifndef XENGINE_RIGIDBODYBT3_HPP
#define XENGINE_RIGIDBODYBT3_HPP

#include "xng/physics/rigidbody.hpp"

namespace xng {
    class RigidBodyBt3 : public RigidBody {
    public:
        void setRigidBodyType(RigidBodyType type) override;

        RigidBodyType getRigidBodyType() override;

        void setPosition(const Vec3f &position) override;

        Vec3f getPosition() override;

        void setVelocity(const Vec3f &velocity) override;

        Vec3f getVelocity() override;

        void setRotation(const Vec3f &rotation) override;

        Vec3f getRotation() override;

        void setAngularVelocity(const Vec3f &angularVelocity) override;

        Vec3f getAngularVelocity() override;

        void applyForce(const Vec3f &force, const Vec3f &point) override;

        void applyTorque(const Vec3f &torque) override;

        void setLockedRotationAxes(const Vec3b &ax) override;

        std::unique_ptr<Collider> createCollider(const ColliderDesc &desc) override;

        void applyLinearImpulse(const Vec3f &impulse, const Vec3f &point) override;

        void applyAngularImpulse(const Vec3f &impulse) override;

        float getMass() override;

        void setGravityScale(float scale) override;
    };
}

#endif //XENGINE_RIGIDBODYBT3_HPP
