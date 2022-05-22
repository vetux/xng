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

#ifndef XENGINE_RIGIDBODYBOX2D_HPP
#define XENGINE_RIGIDBODYBOX2D_HPP

#include "physics/2d/rigidbody2d.hpp"

#include "colliderbox2d.hpp"

namespace xengine {
    class WorldBox2D;

    class RigidBodyBox2D : public RigidBody2D {
    public:
        b2Body *body{};
        std::set<ColliderBox2D *> colliders;

        RigidBodyBox2D() = default;

        explicit RigidBodyBox2D(b2World &world);

        ~RigidBodyBox2D() override;

        void setRigidBodyType(RigidBodyType type) override;

        RigidBodyType getRigidBodyType() override;

        void setPosition(const Vec2f &position) override;

        Vec2f getPosition() override;

        void setVelocity(const Vec2f &velocity) override;

        Vec2f getVelocity() override;

        void setRotation(float rotation) override;

        float getRotation() override;

        void setAngularVelocity(float angularVelocity) override;

        float getAngularVelocity() override;

        Collider2D *createCollider() override;

        void destroyCollider(Collider2D *collider) override;

        std::set<Collider2D *> getColliders() override;
    };
}

#endif //XENGINE_RIGIDBODYBOX2D_HPP
