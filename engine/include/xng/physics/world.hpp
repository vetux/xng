/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_WORLD3D_HPP
#define XENGINE_WORLD3D_HPP

#include "rigidbody.hpp"
#include "joint.hpp"
#include "collidershape.hpp"

namespace xng {
    class XENGINE_EXPORT World {
    public:
        struct XENGINE_EXPORT Contact {
            std::reference_wrapper<Collider> colliderA;
            std::reference_wrapper<Collider> colliderB;
        };

        class XENGINE_EXPORT ContactListener {
        public:
            virtual ~ContactListener() = default;

            virtual void beginContact(Contact &contact) {}

            virtual void endContact(Contact &contact) {}
        };

        virtual ~World() = default;

        virtual std::unique_ptr<RigidBody> createBody() = 0;

        virtual std::unique_ptr<Joint> createJoint() = 0;

        virtual void addContactListener(ContactListener &listener) = 0;

        virtual void removeContactListener(ContactListener &listener) = 0;

        virtual void setGravity(const Vec3f &gravity) = 0;

        virtual void step(float deltaTime) = 0;
    };
}

#endif //XENGINE_WORLD3D_HPP
