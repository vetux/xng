/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_WORLDBOX2D_HPP
#define XENGINE_WORLDBOX2D_HPP

#include "box2dinclude.hpp"

#include "xng/physics/world.hpp"
#include "xng/physics/rigidbody.hpp"

#include "physics/box2d/rigidbodybox2d.hpp"
#include "physics/box2d/jointbox2d.hpp"
#include "xng/util/time.hpp"

namespace xng {
    namespace box2d {
        class WorldBox2D : public World, public b2ContactListener {
        public:
            b2World world;

            std::set<World::ContactListener *> contactListeners;

            std::map<b2Fixture *, ColliderBox2D *> fixtureColliderMapping;

            float timeStep = 1.0f / 30; // The duration of one physics world step
            float deltaAccumulator = 0;

            WorldBox2D();

            ~WorldBox2D() override;

            std::unique_ptr<RigidBody> createBody() override;

            std::unique_ptr<RigidBody>
            createBody(const ColliderDesc &colliderDesc, RigidBody::RigidBodyType type) override;

            std::unique_ptr<Joint> createJoint() override;

            void addContactListener(ContactListener &listener) override;

            void removeContactListener(ContactListener &listener) override;

            void setGravity(const Vec3f &gravity) override;

            void step(DeltaTime deltaTime) override;

            void step(DeltaTime deltaTime, int maxSteps) override;

            std::vector<RayHit> rayTestAll(const Vec3f &from, const Vec3f &to) override;

            RayHit rayTestClosest(const Vec3f &from, const Vec3f &to) override;


            void BeginContact(b2Contact *contact) override;

            void EndContact(b2Contact *contact) override;
        };
    }
}

#endif //XENGINE_WORLDBOX2D_HPP
