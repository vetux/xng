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

#include "rigidbodybt3.hpp"

namespace xng {
    void RigidBodyBt3::setRigidBodyType(RigidBody::RigidBodyType type) {

    }

    RigidBody::RigidBodyType RigidBodyBt3::getRigidBodyType() {
        return DYNAMIC;
    }

    void RigidBodyBt3::setPosition(const Vec3f &position) {

    }

    Vec3f RigidBodyBt3::getPosition() {
        return Vec3f();
    }

    void RigidBodyBt3::setVelocity(const Vec3f &velocity) {

    }

    Vec3f RigidBodyBt3::getVelocity() {
        return Vec3f();
    }

    void RigidBodyBt3::setRotation(const Vec3f &rotation) {

    }

    Vec3f RigidBodyBt3::getRotation() {
        return Vec3f();
    }

    void RigidBodyBt3::setAngularVelocity(const Vec3f &angularVelocity) {

    }

    Vec3f RigidBodyBt3::getAngularVelocity() {
        return Vec3f();
    }

    std::unique_ptr<Collider> RigidBodyBt3::createCollider(const ColliderDesc &desc) {
        return std::unique_ptr<Collider>();
    }

    void RigidBodyBt3::applyForce(const Vec3f &force, const Vec3f &point) {
    }

    void RigidBodyBt3::applyTorque(const Vec3f &torque) {
    }

    void RigidBodyBt3::setLockedRotationAxes(const Vec3b &ax) {

    }
}