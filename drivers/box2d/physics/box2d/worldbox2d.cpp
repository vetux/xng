/**
 *  xEngine - C++ Game Engine Library
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

#include "worldbox2d.hpp"
#include "rigidbodybox2d.hpp"
#include "commonbox2d.hpp"

namespace xng {
    namespace box2d {
        WorldBox2D::WorldBox2D()
                : world(b2Vec2(0.0f, -1.0f)) {
            world.SetContactListener(this);
        }

        WorldBox2D::~WorldBox2D() {
            world.SetContactListener(nullptr);
        }

        std::unique_ptr<RigidBody> WorldBox2D::createBody() {
            return std::make_unique<RigidBodyBox2D>(*this);
        }

        std::unique_ptr<Joint> WorldBox2D::createJoint() {
            throw std::runtime_error("Not Implemented");
        }

        void WorldBox2D::addContactListener(World::ContactListener &listener) {
            contactListeners.insert(&listener);
        }

        void WorldBox2D::removeContactListener(World::ContactListener &listener) {
            contactListeners.erase(&listener);
        }

        void WorldBox2D::setGravity(const Vec3f &gravity) {
            world.SetGravity(convert(gravity));
        }

        void WorldBox2D::step(float deltaTime) {
            world.Step(deltaTime, 10, 10);
        }

        void WorldBox2D::BeginContact(b2Contact *c) {
            Contact contact = Contact{.colliderA = *fixtureColliderMapping.at(c->GetFixtureA()),
                    .colliderB = *fixtureColliderMapping.at(c->GetFixtureB())};
            for (auto &listener: contactListeners) {
                listener->beginContact(contact);
            }
        }

        void WorldBox2D::EndContact(b2Contact *c) {
            Contact contact = Contact{.colliderA = *fixtureColliderMapping.at(c->GetFixtureA()),
                    .colliderB = *fixtureColliderMapping.at(c->GetFixtureB())};
            for (auto &listener: contactListeners) {
                listener->endContact(contact);
            }
        }
    }
}