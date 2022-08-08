/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include <event/events/contactevent.hpp>
#include "ecs/systems/physicssystem.hpp"

#include "ecs/components.hpp"

namespace xng {
    PhysicsSystem::PhysicsSystem(World &world, EventBus &eventBus)
            : world(world), eventBus(eventBus) {}

    void PhysicsSystem::start(EntityScene &scene) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (rigidbodies.find(pair.first) == rigidbodies.end()) {
                onComponentCreate(pair.first, pair.second, typeid(RigidBodyComponent));
            }
        }
        scene.addListener(*this);
    }

    void PhysicsSystem::stop(EntityScene &scene) {
        scene.removeListener(*this);
    }

    void PhysicsSystem::update(DeltaTime deltaTime, EntityScene &scene) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (pair.second.syncTransform) {
                auto &rb = *rigidbodies.at(pair.first).get();
                auto tcomp = scene.lookup<TransformComponent>(pair.first);
                rb.setPosition(tcomp.transform.getPosition());
                rb.setRotation(tcomp.transform.getRotation().getEulerAngles());
                rb.applyForce(pair.second.force, pair.second.forcePoint);
                rb.applyTorque(pair.second.torque);
            }
        }

        world.step(deltaTime);

        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (pair.second.syncTransform) {
                auto &rb = *rigidbodies.at(pair.first).get();
                auto tcomp = scene.lookup<TransformComponent>(pair.first);

                tcomp.transform.setPosition(rb.getPosition());
                tcomp.transform.setRotation(Quaternion(rb.getRotation()));

                RigidBodyComponent comp = pair.second;
                comp.force = Vec3f();
                comp.torque = Vec3f();
                scene.updateComponent(pair.first, comp);
            }
        }
    }

    void PhysicsSystem::onComponentCreate(const EntityHandle &entity,
                                          const std::any &component,
                                          std::type_index componentType) {
        if (componentType == typeid(RigidBodyComponent)) {
            auto &comp = *std::any_cast<RigidBodyComponent>(&component);
            auto body = world.createBody();

            body->setRigidBodyType(comp.type);

            for (auto i = 0; i < comp.colliders.size(); i++) {
                auto collider = body->createCollider(comp.colliders.at(i));
                colliderIndices[collider.get()] = i;
                colliders[entity].emplace_back(std::move(collider));
            }

            rigidbodiesReverse[body.get()] = entity;
            rigidbodies[entity] = std::move(body);
        }
    }

    void PhysicsSystem::onComponentDestroy(const EntityHandle &entity,
                                           const std::any &component,
                                           std::type_index componentType) {
        if (componentType == typeid(RigidBodyComponent)) {
            auto &comp = *std::any_cast<RigidBodyComponent>(&component);
            for (auto &col: colliders.at(entity)) {
                colliderIndices.erase(col.get());
            }
            colliders.erase(entity);
            rigidbodies.erase(entity);
        }
    }

    void PhysicsSystem::onComponentUpdate(const EntityHandle &entity,
                                          const std::any &oldComponent,
                                          const std::any &newComponent,
                                          std::type_index componentType) {
        if (componentType == typeid(RigidBodyComponent)) {
            auto &oComp = *std::any_cast<RigidBodyComponent>(&oldComponent);
            auto &nComp = *std::any_cast<RigidBodyComponent>(&newComponent);

            rigidbodies.at(entity)->setRigidBodyType(nComp.type);

            if (oComp.colliders != nComp.colliders) {
                for (auto &col: colliders.at(entity)) {
                    colliderIndices.erase(col.get());
                }
                colliders.erase(entity);

                for (auto i = 0; i < nComp.colliders.size(); i++) {
                    auto collider = rigidbodies.at(entity)->createCollider(nComp.colliders.at(i));
                    colliderIndices[collider.get()] = i;
                    colliders[entity].emplace_back(std::move(collider));
                }
            }
        }
    }

    void PhysicsSystem::beginContact(World::Contact &contact) {
        auto entA = rigidbodiesReverse.at(&contact.colliderA.get().getBody());
        auto entB = rigidbodiesReverse.at(&contact.colliderB.get().getBody());
        auto indexA = colliderIndices.at(&contact.colliderA.get());
        auto indexB = colliderIndices.at(&contact.colliderB.get());
        eventBus.invoke(ContactEvent(ContactEvent::BEGIN_CONTACT,
                                     entA,
                                     entB,
                                     indexA,
                                     indexB));
    }

    void PhysicsSystem::endContact(World::Contact &contact) {
        auto entA = rigidbodiesReverse.at(&contact.colliderA.get().getBody());
        auto entB = rigidbodiesReverse.at(&contact.colliderB.get().getBody());
        auto indexA = colliderIndices.at(&contact.colliderA.get());
        auto indexB = colliderIndices.at(&contact.colliderB.get());
        eventBus.invoke(ContactEvent(ContactEvent::END_CONTACT,
                                     entA,
                                     entB,
                                     indexA,
                                     indexB));
    }
}