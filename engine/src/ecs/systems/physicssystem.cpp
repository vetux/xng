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

    PhysicsSystem::PhysicsSystem(World &world, EventBus &eventBus, float scale, float timeStep)
            : world(world), eventBus(eventBus), scale(scale), timeStep(timeStep) {}

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
        deltaAccumulator += deltaTime;
        int steps = static_cast<int>(deltaAccumulator / timeStep);
        deltaAccumulator -= timeStep * static_cast<float>(steps);

        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            auto &rb = *rigidbodies.at(pair.first).get();
            auto tcomp = scene.getComponent<TransformComponent>(pair.first);
            rb.setPosition(tcomp.transform.getPosition() / scale);
            rb.setRotation(tcomp.transform.getRotation().getEulerAngles());
            rb.setVelocity(pair.second.velocity);
            rb.setAngularVelocity(pair.second.angularVelocity);
            if (pair.second.mass > 0){
                rb.setMass(pair.second.mass, pair.second.massCenter, pair.second.rotationalInertia);
            }
        }

        for (int i = 0; i < steps; i++) {
            for (auto &pair: scene.getPool<RigidBodyComponent>()) {
                auto &rb = *rigidbodies.at(pair.first).get();
                rb.applyForce(pair.second.force, pair.second.forcePoint / scale);
                rb.applyTorque(pair.second.torque);
                rb.applyLinearImpulse(pair.second.impulse, pair.second.impulsePoint);
                rb.applyAngularImpulse(pair.second.angularImpulse);
            }

            world.step(timeStep);
        }

        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            auto &rb = *rigidbodies.at(pair.first).get();
            auto tcomp = scene.getComponent<TransformComponent>(pair.first);

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

            comp.touchingColliders.clear();
            for (int i = 0; i < comp.colliders.size(); i++) {
                auto it = touchingColliders.find(std::pair(pair.first, i));
                if (it != touchingColliders.end()) {
                    for (auto &entIndexPair: it->second) {
                        comp.touchingColliders[entIndexPair.first].insert(entIndexPair.second);
                    }
                }
            }

            scene.updateComponent(pair.first, comp);
            scene.updateComponent(pair.first, tcomp);
        }
    }

    void PhysicsSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(RigidBodyComponent)) {
            auto &comp = dynamic_cast<const RigidBodyComponent &>(component);
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

    void PhysicsSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(RigidBodyComponent)) {
            auto &comp = dynamic_cast<const RigidBodyComponent &>(component);
            for (auto &col: colliders.at(entity)) {
                colliderIndices.erase(col.get());
            }
            colliders.erase(entity);
            rigidbodiesReverse.erase(rigidbodies.at(entity).get());
            rigidbodies.erase(entity);
        }
    }

    void PhysicsSystem::onComponentUpdate(const EntityHandle &entity,
                                          const Component &oldComponent,
                                          const Component &newComponent) {
        if (oldComponent.getType() == typeid(RigidBodyComponent)) {
            auto &oComp = dynamic_cast<const RigidBodyComponent &>(oldComponent);
            auto &nComp = dynamic_cast<const RigidBodyComponent &>(newComponent);

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

            rigidbodies.at(entity)->setRigidBodyType(nComp.type);
            rigidbodies.at(entity)->setLockedRotationAxes(nComp.lockedAxes);
            rigidbodies.at(entity)->setGravityScale(nComp.gravityScale);
        }
    }

    void PhysicsSystem::beginContact(World::Contact &contact) {
        auto entA = rigidbodiesReverse.at(&contact.colliderA.get().getBody());
        auto entB = rigidbodiesReverse.at(&contact.colliderB.get().getBody());
        auto indexA = colliderIndices.at(&contact.colliderA.get());
        auto indexB = colliderIndices.at(&contact.colliderB.get());

        touchingColliders[std::pair(entA, indexA)].insert(std::pair(entB, indexB));
        touchingColliders[std::pair(entB, indexB)].insert(std::pair(entA, indexA));

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

        touchingColliders[std::pair(entA, indexA)].erase(std::pair(entB, indexB));
        touchingColliders[std::pair(entB, indexB)].erase(std::pair(entA, indexA));

        eventBus.invoke(ContactEvent(ContactEvent::END_CONTACT,
                                     entA,
                                     entB,
                                     indexA,
                                     indexB));
    }

    void PhysicsSystem::onEntityDestroy(const EntityHandle &entity) {
        auto itc = colliders.find(entity);
        if (itc != colliders.end()) {
            std::set<Collider *> ptrs;
            for (auto &c: itc->second)
                ptrs.insert(c.get());
            colliders.erase(entity);
            for (auto &c: ptrs)
                colliderIndices.erase(c);
        }
        auto it = rigidbodies.find(entity);
        if (it != rigidbodies.end()) {
            auto ptr = it->second.get();
            rigidbodies.erase(entity);
            rigidbodiesReverse.erase(ptr);
        }
    }
}