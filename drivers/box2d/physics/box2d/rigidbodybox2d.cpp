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

#include "rigidbodybox2d.hpp"
#include "commonbox2d.hpp"

namespace xng {
    RigidBodyBox2D::RigidBodyBox2D(b2World &world) {
        b2BodyDef def;
        body = world.CreateBody(&def);
    }

    RigidBodyBox2D::~RigidBodyBox2D() {
        body->GetWorld()->DestroyBody(body);
    }

    void RigidBodyBox2D::setRigidBodyType(RigidBodyType type) {
        body->SetType(convert(type));
    }

    RigidBody::RigidBodyType RigidBodyBox2D::getRigidBodyType() {
        return convert(body->GetType());
    }

    void RigidBodyBox2D::setPosition(const Vec3f &position) {
        body->SetTransform(convert(position), 0);
    }

    Vec3f RigidBodyBox2D::getPosition() {
        return convert(body->GetPosition());
    }

    void RigidBodyBox2D::setVelocity(const Vec3f &velocity) {
        body->SetLinearVelocity(convert(velocity));
    }

    Vec3f RigidBodyBox2D::getVelocity() {
        return convert(body->GetLinearVelocity());
    }

    void RigidBodyBox2D::setRotation(const Vec3f &rotation) {
        body->SetTransform(body->GetPosition(), rotation.z);
    }

    Vec3f RigidBodyBox2D::getRotation() {
        return {0, 0, body->GetAngle()};
    }

    void RigidBodyBox2D::setAngularVelocity(const Vec3f &angularVelocity) {
        body->SetAngularVelocity(angularVelocity.z);
    }

    Vec3f RigidBodyBox2D::getAngularVelocity() {
        return {0, 0, body->GetAngularVelocity()};
    }

    void RigidBodyBox2D::setColliders(std::vector<std::reference_wrapper<Collider>> colliders) {}
}