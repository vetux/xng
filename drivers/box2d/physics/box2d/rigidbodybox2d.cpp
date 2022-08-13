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
#include "physics/box2d/worldbox2d.hpp"
#include "math/rotation.hpp"

namespace xng {
    RigidBodyBox2D::RigidBodyBox2D(WorldBox2D &world)
            : world(world) {
        b2BodyDef def;
        body = world.world.CreateBody(&def);
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
        body->SetTransform(body->GetPosition(), degreesToRadians(rotation.z));
    }

    Vec3f RigidBodyBox2D::getRotation() {
        return {0, 0, radiansToDegrees(body->GetAngle())};
    }

    void RigidBodyBox2D::setAngularVelocity(const Vec3f &angularVelocity) {
        body->SetAngularVelocity(angularVelocity.z);
    }

    Vec3f RigidBodyBox2D::getAngularVelocity() {
        return {0, 0, body->GetAngularVelocity()};
    }

    void RigidBodyBox2D::applyForce(const Vec3f &force, const Vec3f &point) {
        body->ApplyForce(convert(force), convert(point), true);
    }

    void RigidBodyBox2D::applyTorque(const Vec3f &torque) {
        body->ApplyTorque(torque.z, true);
    }

    void RigidBodyBox2D::applyLinearImpulse(const Vec3f &impulse, const Vec3f &point) {
        body->ApplyLinearImpulse(convert(impulse), convert(point), true);
    }

    void RigidBodyBox2D::applyAngularImpulse(const Vec3f &impulse) {
        body->ApplyAngularImpulse(impulse.z, true);
    }

    std::unique_ptr<Collider> RigidBodyBox2D::createCollider(const ColliderDesc &desc) {
        return std::make_unique<ColliderBox2D>(*this, desc);
    }

    void RigidBodyBox2D::setLockedRotationAxes(const Vec3b &ax) {
        body->SetFixedRotation(ax.z);
    }

    float RigidBodyBox2D::getMass() {
        return body->GetMass();
    }
}