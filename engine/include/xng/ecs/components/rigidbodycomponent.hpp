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

#ifndef XENGINE_RIGIDBODYCOMPONENT_HPP
#define XENGINE_RIGIDBODYCOMPONENT_HPP

#include "xng/physics/rigidbody.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT RigidBodyComponent : public Component {
        RigidBody::RigidBodyType type;

        Vec3b lockedAxes;

        Vec3f velocity;
        Vec3f angularVelocity;

        Vec3f force = Vec3f();
        Vec3f forcePoint = Vec3f();
        Vec3f torque = Vec3f();

        Vec3f impulse = Vec3f();
        Vec3f impulsePoint = Vec3f();
        Vec3f angularImpulse = Vec3f();

        float mass = -1;
        Vec3f massCenter = Vec3f();
        Vec3f rotationalInertia = Vec3f();

        float gravityScale = 1;

        std::vector<ResourceHandle<ColliderDesc>> colliders;

        std::map<EntityHandle, std::set<int>> touchingColliders; // Updated at runtime by the physics system, for every touching entity the indices of the touching collider

        RigidBody::RigidBodyType convert(const std::string &text) const {
            if (text == "static")
                return RigidBody::STATIC;
            else if (text == "kinematic")
                return RigidBody::KINEMATIC;
            else
                return RigidBody::DYNAMIC;
        }

        std::string convert(RigidBody::RigidBodyType type) const {
            switch (type) {
                case RigidBody::STATIC:
                    return "static";
                case RigidBody::KINEMATIC:
                    return "kinematic";
                case RigidBody::DYNAMIC:
                default:
                    return "dynamic";
            }
        }

        Messageable &operator<<(const Message &message) override {
            type = convert(message.value("type", std::string("static")));
            lockedAxes << message.value("lockedAxes");
            velocity << message.value("velocity");
            angularVelocity << message.value("angularVelocity");
            mass = message.value("mass", -1);
            massCenter << message.value("massCenter");
            rotationalInertia << message.value("rotationalInertia");
            if (message.has("colliders") && message.value("colliders").getType() == Message::LIST) {
                for (auto &col: message.value("colliders").asList()) {
                    ResourceHandle<ColliderDesc> desc;
                    desc << col;
                    colliders.emplace_back(desc);
                }
            }
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message["type"] = convert(type);
            lockedAxes >> message["lockedAxes"];
            velocity >> message["velocity"];
            angularVelocity >> message["angularVelocity"];
            message["mass"] = mass;
            massCenter >> message["massCenter"];
            rotationalInertia >> message["rotationalInertia"];
            auto vec = std::vector<Message>();
            for (auto &col: colliders) {
                Message msg;
                col >> msg;
                vec.emplace_back(msg);
            }
            message["colliders"] = vec;
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(RigidBodyComponent);
        }
    };
}

#endif //XENGINE_RIGIDBODYCOMPONENT_HPP
