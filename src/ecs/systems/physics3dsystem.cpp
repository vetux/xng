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

#include "ecs/systems/physics3dsystem.hpp"

#include "ecs/components.hpp"

namespace xengine {
    /*struct RigidBodyData3D : public Component::UserData {
        World3D *world;
        RigidBody3D *body;

        ~RigidBodyData3D() override {
            world->destroyRigidBody(body);
        }
    };

    struct ColliderData3D : public Component::UserData {
        RigidBody3D *body;
        Collider3D *collider;

        ~ColliderData3D() override {
            body->destroyCollider(collider);
        }
    };

    Physics3DSystem::Physics3DSystem(World3D &world) : world(&world) {}

    void Physics3DSystem::start() {}

    void Physics3DSystem::stop() {}

    void Physics3DSystem::update(float deltaTime, AssetScene &scene) {
        for (auto &node : scene.findNodesWithComponent<RigidBodyComponent3D>()) {
            auto &transform = node->getComponent<TransformComponent>();
            auto &rb = node->getComponent<RigidBodyComponent3D>();

            if (rb.userData == nullptr) {
                auto *data = new RigidBodyData3D();
                data->world = world;
                data->body = world->createRigidBody();
                data->body->setRigidBodyType(rb.type);
                rb.userData = std::unique_ptr<RigidBodyData3D>(data);
            }

            auto &rbData = *dynamic_cast<RigidBodyData3D *>(rb.userData.get());

            //Apply rigid body
            rbData.body->setRigidBodyType(rb.type);

            //Apply transform
            rbData.body->setPosition(transform.transform.position);
            rbData.body->setRotation(transform.transform.rotation);

            if (!node->hasComponent<ColliderComponent3D>())
                continue;;

            auto &collider = node->getComponent<ColliderComponent3D>();

            if (collider.userData == nullptr) {
                auto *data = new ColliderData3D();
                data->body = rbData.body;
                data->collider = rbData.body->createCollider();
                collider.userData = std::unique_ptr<ColliderData3D>(data);
            }

            auto &colData = *dynamic_cast<ColliderData3D *>(collider.userData.get());

            //Apply collider
            colData.collider->setShape(collider.shape);
        }

        world->step(deltaTime);

        for (auto &node : scene.findNodesWithComponent<RigidBodyComponent2D>()) {
            auto &transform = node->getComponent<TransformComponent>();
            auto &rb = node->getComponent<RigidBodyComponent2D>();

            auto &rbData = *dynamic_cast<RigidBodyData3D *>(rb.userData.get());

            //Synchronize transform
            transform.transform.position = rbData.body->getPosition();
            transform.transform.rotation = rbData.body->getRotation();
        }
    }*/
}