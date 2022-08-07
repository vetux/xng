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

#include "worldbt3.hpp"

namespace xng {
    std::unique_ptr<RigidBody> WorldBt3::createBody() {
        return std::unique_ptr<RigidBody>();
    }

    std::unique_ptr<Joint> WorldBt3::createJoint() {
        return std::unique_ptr<Joint>();
    }

    void WorldBt3::addContactListener(World::ContactListener &listener) {

    }

    void WorldBt3::removeContactListener(World::ContactListener &listener) {

    }

    void WorldBt3::setGravity(const Vec3f &gravity) {

    }

    void WorldBt3::step(float deltaTime) {

    }
}