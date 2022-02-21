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

#ifndef XENGINE_WORLD3D_HPP
#define XENGINE_WORLD3D_HPP

#include "physics/3d/rigidbody3d.hpp"
#include "physics/3d/joint3d.hpp"

namespace xengine {
    class XENGINE_EXPORT World3D {
    public:
        enum Backend {
            BULLET3
        };

        static std::unique_ptr<World3D> create(Backend backend);

        struct XENGINE_EXPORT Contact {
            Collider3D *colliderA;
            Collider3D *colliderB;

            std::vector<Vec3f> points;
        };

        class XENGINE_EXPORT ContactListener {
        public:
            virtual ~ContactListener() = default;

            virtual void beginContact(Contact &contact) {}

            virtual void endContact(Contact &contact) {}
        };

        virtual ~World3D() = default;

        virtual RigidBody3D *createRigidBody() = 0;

        virtual void destroyRigidBody(RigidBody3D *ptr) = 0;

        virtual std::set<RigidBody3D *> getRigidBodies() = 0;

        virtual Joint3D *createJoint() = 0;

        virtual void destroyJoint(Joint3D *ptr) = 0;

        virtual std::set<Joint3D *> getJoints() = 0;

        virtual void addContactListener(ContactListener *listener) = 0;

        virtual void removeContactListener(ContactListener *listener) = 0;

        virtual void setGravity(const Vec3f &gravity) = 0;

        virtual void step(float deltaTime) = 0;
    };
}

#endif //XENGINE_WORLD3D_HPP
