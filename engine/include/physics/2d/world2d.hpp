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

#ifndef XENGINE_WORLD_HPP
#define XENGINE_WORLD_HPP

#include "physics/2d/rigidbody2d.hpp"
#include "physics/2d/joint2d.hpp"

#include "driver/driver.hpp"

namespace xengine {
    class XENGINE_EXPORT World2D : public Driver {
    public:
        struct XENGINE_EXPORT Contact {
            Collider2D *colliderA;
            Collider2D *colliderB;

            std::vector<Vec2f> points;
        };

        class XENGINE_EXPORT ContactListener {
        public:
            virtual ~ContactListener() = default;

            virtual void beginContact(Contact &contact) {}

            virtual void endContact(Contact &contact) {}
        };

        ~World2D() override = default;

        virtual RigidBody2D *createRigidBody() = 0;

        virtual void destroyRigidBody(RigidBody2D *ptr) = 0;

        virtual std::set<RigidBody2D *> getRigidBodies() = 0;

        virtual Joint2D *createJoint() = 0;

        virtual void destroyJoint(Joint2D *ptr) = 0;

        virtual std::set<Joint2D *> getJoints() = 0;

        virtual void addContactListener(ContactListener *listener) = 0;

        virtual void removeContactListener(ContactListener *listener) = 0;

        virtual void setGravity(const Vec2f &gravity) = 0;

        virtual void step(float deltaTime) = 0;
    };
}

#endif //XENGINE_WORLD_HPP
