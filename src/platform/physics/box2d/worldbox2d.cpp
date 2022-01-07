/**
 *  XEngine - C++ game engine library
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

#include "worldbox2d.hpp"
#include "rigidbodybox2d.hpp"
#include "commonbox2d.hpp"

namespace xengine {
    WorldBox2D::WorldBox2D()
            : world(b2Vec2(0.0f, -1.0f)) {}

    WorldBox2D::~WorldBox2D() {
        for (auto *p : bodies) {
            delete p;
        }
        for (auto *p : joints) {
            delete p;
        }
    }

    RigidBody2D *WorldBox2D::createRigidBody() {
        auto *ret = new RigidBodyBox2D(world);
        bodies.insert(ret);
        return ret;
    }

    void WorldBox2D::destroyRigidBody(RigidBody2D *body) {
        auto it = bodies.find(dynamic_cast<RigidBodyBox2D *>(body));
        if (it != bodies.end()) {
            delete *it;
            bodies.erase(it);
        }
    }

    std::set<RigidBody2D *> WorldBox2D::getRigidBodies() {
        std::set<RigidBody2D *> ret;
        for (auto *ptr : bodies)
            ret.insert(dynamic_cast<RigidBody2D *>(ptr));
        return ret;
    }

    Joint2D *WorldBox2D::createJoint() {
        auto *ret = new JointBox2D();
        joints.insert(ret);
        return ret;
    }

    void WorldBox2D::destroyJoint(Joint2D *joint) {
        auto it = joints.find(dynamic_cast<JointBox2D *>(joint));
        if (it != joints.end()) {
            delete *it;
            joints.erase(it);
        }
    }

    std::set<Joint2D *> WorldBox2D::getJoints() {
        std::set<Joint2D *> ret;
        for (auto *ptr : joints)
            ret.insert(dynamic_cast<Joint2D *>(ptr));
        return ret;
    }

    void WorldBox2D::addContactListener(ContactListener *listener) {
        throw std::runtime_error("Not implemented");
    }

    void WorldBox2D::removeContactListener(ContactListener *listener) {
        throw std::runtime_error("Not implemented");
    }

    void WorldBox2D::setGravity(const Vec2f &gravity) {
        world.SetGravity(convert(gravity));
    }

    void WorldBox2D::step(float deltaTime) {
        world.Step(deltaTime, 10, 10);
    }
}

#endif