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

#include "ecs/systems/physics2dsystem.hpp"

#include "ecs/components.hpp"

namespace xng {
   /* struct RigidBodyData {
        World2D *world;
        RigidBody2D *body;

        ~RigidBodyData() override {
            world->destroyRigidBody(body);
        }
    };

    struct ColliderData : public Component::UserData {
        RigidBody2D *body;
        Collider2D *collider;

        ~ColliderData() override {
            body->destroyCollider(collider);
        }
    };

    Physics2DSystem::Physics2DSystem(World2D &world) : world(&world) {}

    void Physics2DSystem::start() {}

    void Physics2DSystem::stop() {}

    void Physics2DSystem::update(float deltaTime, AssetScene &scene) {
        for (auto &node : scene.findNodesWithComponent<RigidBodyComponent2D>()) {
            auto &transform = node->getComponent<TransformComponent>();
            auto &rb = node->getComponent<RigidBodyComponent2D>();

            if (rb.userData == nullptr) {
                auto *data = new RigidBodyData();
                data->world = world;
                data->body = world->createRigidBody();
                data->body->setRigidBodyType(rb.type);
                rb.userData = std::unique_ptr<RigidBodyData>(data);
            }

            auto &rbData = *dynamic_cast<RigidBodyData *>(rb.userData.get());

            //Apply rigid body
            rbData.body->setRigidBodyType(rb.type);

            //Apply transform
            rbData.body->setPosition(Vec2f(transform.transform.position.x, transform.transform.position.y));
            rbData.body->setRotation(transform.transform.rotation.z);

            if (!node->hasComponent<ColliderComponent2D>())
                continue;;

            auto &collider = node->getComponent<ColliderComponent2D>();

            if (collider.userData == nullptr) {
                auto *data = new ColliderData();
                data->body = rbData.body;
                data->collider = rbData.body->createCollider();
                collider.userData = std::unique_ptr<ColliderData>(data);
            }

            auto &colData = *dynamic_cast<ColliderData *>(collider.userData.get());

            //Apply collider
            colData.collider->setShape(collider.shape);
        }

        world->step(deltaTime);

        for (auto &node : scene.findNodesWithComponent<RigidBodyComponent2D>()) {
            auto &transform = node->getComponent<TransformComponent>();
            auto &rb = node->getComponent<RigidBodyComponent2D>();

            auto &rbData = *dynamic_cast<RigidBodyData *>(rb.userData.get());

            //Synchronize transform
            auto position = rbData.body->getPosition();
            transform.transform.position = Vec3f(position.x, position.y, 0);
            transform.transform.rotation = Vec3f(0, 0, rbData.body->getRotation());
        }
    }*/
}