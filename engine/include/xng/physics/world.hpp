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

#ifndef XENGINE_WORLD3D_HPP
#define XENGINE_WORLD3D_HPP

#include "xng/physics/rigidbody.hpp"
#include "xng/physics/joint.hpp"
#include "xng/physics/colliderdesc.hpp"
#include "xng/physics/rigidbody.hpp"
#include "xng/physics/rayhit.hpp"

namespace xng {
    class XENGINE_EXPORT World {
    public:
        struct XENGINE_EXPORT Contact {
            std::reference_wrapper<Collider> colliderA;
            std::reference_wrapper<Collider> colliderB;

            Vec3f positionWorldA;
            Vec3f positionWorldB;

            Vec3f normalOnB;

            bool operator==(const Contact &other) const {
                return &colliderA.get() == &other.colliderA.get()
                       && &colliderB.get() == &other.colliderB.get()
                       && positionWorldA == other.positionWorldA
                       && positionWorldB == other.positionWorldB
                       && normalOnB == other.normalOnB;
            }
        };

        class XENGINE_EXPORT ContactListener {
        public:
            virtual ~ContactListener() = default;

            virtual void beginContact(const Contact &contact) {}

            virtual void endContact(const Contact &contact) {}
        };

        virtual ~World() = default;

        /**
         * Create a rigidbody with no colliders.
         *
         * @return
         */
        virtual std::unique_ptr<RigidBody> createBody() = 0;

        /**
         * Create a rigidbody with a collider attached.
         * Certain physics apis (bullet3) do not support attaching colliders at runtime through Rigidbody::createCollider.
         *
         * @param colliderDesc
         * @return
         */
        virtual std::unique_ptr<RigidBody> createBody(const ColliderDesc &colliderDesc, RigidBody::RigidBodyType type) = 0;

        virtual std::unique_ptr<Joint> createJoint() = 0;

        virtual void addContactListener(ContactListener &listener) = 0;

        virtual void removeContactListener(ContactListener &listener) = 0;

        virtual void setGravity(const Vec3f &gravity) = 0;

        virtual void step(float deltaTime) = 0;

        virtual void step(float deltaTime, int maxSteps) = 0;

        virtual std::vector<RayHit> rayTestAll(const Vec3f &from, const Vec3f &to) = 0;

        virtual RayHit rayTestClosest(const Vec3f &from, const Vec3f &to) = 0;
    };
}

namespace std {
    template<>
    struct hash<xng::World::Contact> {
        std::size_t operator()(const xng::World::Contact &k) const {
            size_t ret = 0;
            xng::hash_combine(ret, &k.colliderA.get());
            xng::hash_combine(ret, &k.colliderB.get());
            xng::hash_combine(ret, k.positionWorldA);
            xng::hash_combine(ret, k.positionWorldB);
            xng::hash_combine(ret, k.normalOnB);
            return ret;
        }
    };
}


#endif //XENGINE_WORLD3D_HPP
