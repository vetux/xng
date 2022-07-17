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

#include "worldbox2d.hpp"
#include "rigidbodybox2d.hpp"
#include "commonbox2d.hpp"

#include "driver/driverregistry.hpp"

namespace xng {
    WorldBox2D::WorldBox2D()
            : world(b2Vec2(0.0f, -1.0f)) {}

    WorldBox2D::~WorldBox2D() {}

    std::unique_ptr<Collider> WorldBox2D::createCollider(const ColliderShape &shape) {
        return std::unique_ptr<Collider>();
    }

    std::unique_ptr<RigidBody> WorldBox2D::createRigidBody() {
        return std::unique_ptr<RigidBody>();
    }

    std::unique_ptr<Joint> WorldBox2D::createJoint() {
        return std::unique_ptr<Joint>();
    }

    void WorldBox2D::addContactListener(World::ContactListener &listener) {

    }

    void WorldBox2D::removeContactListener(World::ContactListener &listener) {

    }

    void WorldBox2D::setGravity(const Vec3f &gravity) {
        world.SetGravity(convert(gravity));
    }

    void WorldBox2D::step(float deltaTime) {
        world.Step(deltaTime, 10, 10);
    }
}