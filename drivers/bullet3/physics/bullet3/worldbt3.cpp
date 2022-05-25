/**
 *  xEngine - C++ game engine library
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

#include "worldbt3.hpp"

#include "driver/drivermanager.hpp"

namespace xengine {
    static bool driverRegistered = REGISTER_DRIVER("bullet3", WorldBt3);

    RigidBody3D *WorldBt3::createRigidBody() {
        return nullptr;
    }

    void WorldBt3::destroyRigidBody(RigidBody3D *ptr) {

    }

    std::set<RigidBody3D *> WorldBt3::getRigidBodies() {
        return std::set<RigidBody3D *>();
    }

    Joint3D *WorldBt3::createJoint() {
        return nullptr;
    }

    void WorldBt3::destroyJoint(Joint3D *ptr) {

    }

    std::set<Joint3D *> WorldBt3::getJoints() {
        return std::set<Joint3D *>();
    }

    void WorldBt3::addContactListener(World3D::ContactListener *listener) {

    }

    void WorldBt3::removeContactListener(World3D::ContactListener *listener) {

    }

    void WorldBt3::setGravity(const Vec3f &gravity) {

    }

    void WorldBt3::step(float deltaTime) {

    }

    std::type_index WorldBt3::getType() {
        return typeid(WorldBt3);
    }
}