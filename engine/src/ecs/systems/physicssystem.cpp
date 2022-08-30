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
    static ColliderDesc applyScale(const ColliderDesc &desc, float scale) {
        ColliderDesc ret = desc;
        ret.shape = {};
        ret.shape.primitive = desc.shape.primitive;
        if (desc.shape.indices.empty()) {
            for (auto &vert: desc.shape.vertices) {
                ret.shape.vertices.emplace_back(vert / scale);
            }
        } else {
            for (auto &index: desc.shape.indices) {
                auto &vert = desc.shape.vertices.at(index);
                ret.shape.vertices.emplace_back(vert / scale);
            }
        }
        return ret;
    }

    PhysicsSystem::PhysicsSystem(World &world, EventBus &eventBus, float scale)
            : world(world), eventBus(eventBus), scale(scale) {}

    void PhysicsSystem::start(EntityScene &scene) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (rigidbodies.find(pair.first) == rigidbodies.end()) {
                onComponentCreate(pair.first, pair.second);
            }
        }
        scene.addListener(*this);
        world.addContactListener(*this);
    }

    void PhysicsSystem::stop(EntityScene &scene) {
        world.removeContactListener(*this);
        scene.removeListener(*this);
    }

    void PhysicsSystem::update(DeltaTime deltaTime, EntityScene &scene) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            auto &rb = *rigidbodies.at(pair.first).get();
            auto tcomp = scene.lookup<TransformComponent>(pair.first);
            rb.setPosition(tcomp.transform.getPosition() / scale);
            rb.setRotation(tcomp.transform.getRotation().getEulerAngles());
            rb.setVelocity(pair.second.velocity);
            rb.setAngularVelocity(pair.second.angularVelocity);
            rb.applyForce(pair.second.force, pair.second.forcePoint / scale);
            rb.applyTorque(pair.second.torque);
            rb.applyLinearImpulse(pair.second.impulse, pair.second.impulsePoint);
            rb.applyAngularImpulse(pair.second.angularImpulse);
        }

        world.step(deltaTime);

        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            auto &rb = *rigidbodies.at(pair.first).get();
            auto tcomp = scene.lookup<TransformComponent>(pair.first);

            tcomp.transform.setPosition(rb.getPosition() * scale);
            tcomp.transform.setRotation(Quaternion(rb.getRotation()));

            RigidBodyComponent comp = pair.second;
            comp.force = Vec3f();
            comp.torque = Vec3f();
            comp.impulse = Vec3f();
            comp.angularImpulse = Vec3f();
            comp.velocity = rb.getVelocity();
            comp.angularVelocity = rb.getAngularVelocity();
            comp.mass = rb.getMass();
            scene.updateComponent(pair.first, comp);
            scene.updateComponent(pair.first, tcomp);
        }
    }

    void PhysicsSystem::onComponentCreate(const EntityHandle &entity, const std::any &component) {
        if (component.type() == typeid(RigidBodyComponent)) {
            auto &comp = *std::any_cast<RigidBodyComponent>(&component);
            auto body = world.createBody();

            body->setRigidBodyType(comp.type);
            body->setLockedRotationAxes(comp.lockedAxes);
            body->setGravityScale(comp.gravityScale);

            for (auto i = 0; i < comp.colliders.size(); i++) {
                auto collider = body->createCollider(applyScale(comp.colliders.at(i), scale));
                colliderIndices[collider.get()] = i;
                colliders[entity].emplace_back(std::move(collider));
            }

            rigidbodiesReverse[body.get()] = entity;
            rigidbodies[entity] = std::move(body);
        }
    }

    void PhysicsSystem::onComponentDestroy(const EntityHandle &entity, const std::any &component) {
        if (component.type() == typeid(RigidBodyComponent)) {
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
                                          const std::any &newComponent) {
        if (oldComponent.type() == typeid(RigidBodyComponent)) {
            auto &oComp = *std::any_cast<RigidBodyComponent>(&oldComponent);
            auto &nComp = *std::any_cast<RigidBodyComponent>(&newComponent);

            rigidbodies.at(entity)->setRigidBodyType(nComp.type);
            rigidbodies.at(entity)->setLockedRotationAxes(nComp.lockedAxes);
            rigidbodies.at(entity)->setGravityScale(nComp.gravityScale);

            if (oComp.colliders != nComp.colliders) {
                for (auto &col: colliders.at(entity)) {
                    colliderIndices.erase(col.get());
                }
                colliders.erase(entity);

                for (auto i = 0; i < nComp.colliders.size(); i++) {
                    auto collider = rigidbodies.at(entity)->createCollider(applyScale(nComp.colliders.at(i), scale));
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