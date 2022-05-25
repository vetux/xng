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

#ifndef XENGINE_WORLDBOX2D_HPP
#define XENGINE_WORLDBOX2D_HPP

#include "box2d.hpp"

#include "physics/2d/world2d.hpp"

#include "rigidbodybox2d.hpp"
#include "jointbox2d.hpp"

#include "driver/driver.hpp"

namespace xengine {
    class WorldBox2D : public World2D {
    public:
        b2World world;

        std::set<RigidBodyBox2D *> bodies;
        std::set<JointBox2D *> joints;

        WorldBox2D();

        ~WorldBox2D() override;

        RigidBody2D *createRigidBody() override;

        void destroyRigidBody(RigidBody2D *ptr) override;

        std::set<RigidBody2D *> getRigidBodies() override;

        Joint2D *createJoint() override;

        void destroyJoint(Joint2D *ptr) override;

        std::set<Joint2D *> getJoints() override;

        void addContactListener(ContactListener *listener) override;

        void removeContactListener(ContactListener *listener) override;

        void setGravity(const Vec2f &gravity) override;

        void step(float deltaTime) override;

        std::type_index getType() override;
    };
}

#endif //XENGINE_WORLDBOX2D_HPP
