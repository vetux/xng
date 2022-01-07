/**
 *  Mana - 3D Game Engine
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

#include "rigidbodybox2d.hpp"
#include "commonbox2d.hpp"

namespace xengine {
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

    RigidBody2D::RigidBodyType RigidBodyBox2D::getRigidBodyType() {
        return convert(body->GetType());
    }

    void RigidBodyBox2D::setPosition(const Vec2f &position) {
        body->SetTransform(convert(position), 0);
    }

    Vec2f RigidBodyBox2D::getPosition() {
        return convert(body->GetPosition());
    }

    void RigidBodyBox2D::setVelocity(const Vec2f &velocity) {
        body->SetLinearVelocity(convert(velocity));
    }

    Vec2f RigidBodyBox2D::getVelocity() {
        return convert(body->GetLinearVelocity());
    }

    void RigidBodyBox2D::setRotation(float rotation) {
        body->SetTransform(body->GetPosition(), rotation);
    }

    float RigidBodyBox2D::getRotation() {
        return body->GetAngle();
    }

    void RigidBodyBox2D::setAngularVelocity(float angularVelocity) {
        body->SetAngularVelocity(angularVelocity);
    }

    float RigidBodyBox2D::getAngularVelocity() {
        return body->GetAngularVelocity();
    }

    Collider2D *RigidBodyBox2D::createCollider() {
        auto *ret = new ColliderBox2D(*body);
        colliders.insert(ret);
        return ret;
    }

    void RigidBodyBox2D::destroyCollider(Collider2D *collider) {
        auto it = colliders.find(dynamic_cast<ColliderBox2D *>(collider));
        if (it != colliders.end()) {
            delete *it;
            colliders.erase(it);
        }
    }

    std::set<Collider2D *> RigidBodyBox2D::getColliders() {
        std::set<Collider2D *> ret;
        for (auto &collider: colliders)
            ret.insert(static_cast< Collider2D *>(collider));
        return ret;
    }
}

#endif