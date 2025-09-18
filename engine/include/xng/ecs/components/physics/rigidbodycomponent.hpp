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

#ifndef XENGINE_RIGIDBODYCOMPONENT_HPP
#define XENGINE_RIGIDBODYCOMPONENT_HPP

#include "xng/physics/rigidbody.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT RigidBodyComponent final : Component {
        XNG_COMPONENT_TYPENAME(RigidBodyComponent)

        RigidBody::RigidBodyType type = RigidBody::DYNAMIC;

        Vec3f angularFactor = Vec3f(1);

        Vec3f velocity = Vec3f();
        Vec3f angularVelocity = Vec3f();

        Vec3f force = Vec3f();
        Vec3f forcePoint = Vec3f();
        Vec3f torque = Vec3f();

        Vec3f impulse = Vec3f();
        Vec3f impulsePoint = Vec3f();
        Vec3f angularImpulse = Vec3f();

        float mass = 15;
        Vec3f massCenter = Vec3f();
        Vec3f rotationalInertia = Vec3f(-1);

        float gravityScale = 1;

        Messageable &operator<<(const Message &message) override {
            message.value("type", (int &) type, (int) RigidBody::DYNAMIC);
            message.value("angularFactor", angularFactor);
            message.value("velocity", velocity);
            message.value("angularVelocity", angularVelocity);

            message.value("mass", mass, -1.0f);
            message.value("massCenter", massCenter);
            message.value("rotationalInertia", rotationalInertia);

            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            type >> message["type"];
            angularFactor >> message["angularFactor"];
            velocity >> message["velocity"];
            angularVelocity >> message["angularVelocity"];
            mass >> message["mass"];
            massCenter >> message["massCenter"];
            rotationalInertia >> message["rotationalInertia"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_RIGIDBODYCOMPONENT_HPP
