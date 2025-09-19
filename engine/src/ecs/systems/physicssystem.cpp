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

#include "xng/event/events/contactevent.hpp"

#include "xng/ecs/systems/physicssystem.hpp"
#include "xng/ecs/components.hpp"
#include "xng/ecs/components/physics/meshcollidercomponent.hpp"
#include "xng/util/time.hpp"

namespace xng {
    static ColliderDesc applyScale(const ColliderDesc &desc, float scale) {
        ColliderDesc ret = desc;
        ret.shape = {};
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

    static ColliderShape getShape(const Mesh &mesh) {
        if (mesh.vertexLayout != Mesh::getDefaultVertexLayout()) {
            throw std::runtime_error("Invalid mesh vertex layout");
        }
        ColliderShape shape{};
        for (const auto &v: mesh.vertices) {
            assert(v.buffer.size() >= sizeof(float) * 2);
            float posx = *reinterpret_cast<const float *>(v.buffer.data());
            float posy = *reinterpret_cast<const float *>(v.buffer.data() + sizeof(float));
            float posz = *reinterpret_cast<const float *>(v.buffer.data() + sizeof(float) * 2);
            shape.vertices.emplace_back(posx,
                                        posy,
                                        posz);
        }
        for (auto &i: mesh.indices) {
            shape.indices.emplace_back(i);
        }
        return shape;
    }

    PhysicsSystem::PhysicsSystem(PhysicsEngine &engine, float scale, float timeStep)
        : world(engine.createWorld()), scale(scale), timeStep(timeStep) {
    }

    PhysicsSystem::PhysicsSystem(PhysicsEngine &engine, float scale, int maxSteps)
        : world(engine.createWorld()), scale(scale), maxSteps(maxSteps) {
    }


    void PhysicsSystem::start(EntityScene &scene, EventBus &eventBus) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (rigidBodies.find(pair.entity) == rigidBodies.end()) {
                onComponentCreate(pair.entity, pair.component);
            }
        }
        bus = &eventBus;
        scene.addListener(*this);
        world->addContactListener(*this);
    }

    void PhysicsSystem::stop(EntityScene &scene, EventBus &eventBus) {
        world->removeContactListener(*this);
        scene.removeListener(*this);
        bus = nullptr;
    }

    void PhysicsSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            if (rigidBodies.find(pair.entity) == rigidBodies.end()) {
                if (scene.checkComponent<ColliderComponent>(pair.entity)) {
                    auto &colliderComponent = scene.getComponent<ColliderComponent>(pair.entity);

                    if (colliderComponent.colliders.size() > 1) {
                        auto body = world->createBody();
                        body->setRigidBodyType(pair.component.type);
                        body->setAngularFactor(pair.component.angularFactor);
                        body->setGravityScale(pair.component.gravityScale);

                        rigidBodiesReverse[body.get()] = pair.entity;
                        rigidBodies[pair.entity] = std::move(body);

                        for (auto i = 0; i < colliderComponent.colliders.size(); ++i) {
                            auto collider = rigidBodies.at(pair.entity)->createCollider(
                                applyScale(colliderComponent.colliders.at(i), scale));
                            colliderIndices[collider.get()] = i;
                            colliders[pair.entity].emplace_back(std::move(collider));
                        }
                    } else {
                        ColliderDesc desc = colliderComponent.colliders.at(0);

                        auto body = world->createBody(desc, pair.component.type);
                        body->setAngularFactor(pair.component.angularFactor);
                        body->setGravityScale(pair.component.gravityScale);

                        rigidBodiesReverse[body.get()] = pair.entity;
                        rigidBodies[pair.entity] = std::move(body);
                        colliderIndices[&body->getFixedCollider()] = 0;
                    }
                } else if (scene.checkComponent<MeshColliderComponent>(pair.entity)) {
                    auto &colliderComponent = scene.getComponent<MeshColliderComponent>(pair.entity);

                    ColliderDesc desc;
                    desc.properties = colliderComponent.properties;
                    desc.shape = getShape(colliderComponent.mesh.get());
                    desc.shape.type = colliderComponent.shapeType;

                    auto body = world->createBody(desc, pair.component.type);
                    body->setAngularFactor(pair.component.angularFactor);
                    body->setGravityScale(pair.component.gravityScale);

                    rigidBodiesReverse[body.get()] = pair.entity;
                    rigidBodies[pair.entity] = std::move(body);
                    colliderIndices[&body->getFixedCollider()] = 0;
                } else {
                    auto body = world->createBody();
                    body->setRigidBodyType(pair.component.type);
                    body->setAngularFactor(pair.component.angularFactor);
                    body->setGravityScale(pair.component.gravityScale);

                    rigidBodiesReverse[body.get()] = pair.entity;
                    rigidBodies[pair.entity] = std::move(body);
                }
            }

            auto &rb = *rigidBodies.at(pair.entity).get();
            auto &transformComponent = scene.getComponent<TransformComponent>(pair.entity);

            if (pair.component.rotationalInertia.x < 0
                || pair.component.rotationalInertia.y < 0
                || pair.component.rotationalInertia.z < 0)
                rb.setMass(pair.component.type == RigidBody::STATIC ? 0 : pair.component.mass,
                           pair.component.massCenter);
            else
                rb.setMass(pair.component.type == RigidBody::STATIC ? 0 : pair.component.mass,
                           pair.component.massCenter,
                           pair.component.rotationalInertia);

            rb.setVelocity(pair.component.velocity);
            rb.setAngularVelocity(pair.component.angularVelocity);

            rb.setPosition(transformComponent.transform.getPosition() / scale);
            rb.setRotation(transformComponent.transform.getRotation().getEulerAngles());

            rb.applyForce(pair.component.force, pair.component.forcePoint / scale);
            rb.applyTorque(pair.component.torque);
            rb.applyLinearImpulse(pair.component.impulse, pair.component.impulsePoint);
            rb.applyAngularImpulse(pair.component.angularImpulse);
        }

        if (timeStep == 0) {
            world->step(deltaTime, maxSteps);
        } else {
            deltaAccumulator += static_cast<float>(deltaTime);
            int steps = static_cast<int>(deltaAccumulator / timeStep);
            deltaAccumulator -= timeStep * static_cast<float>(steps);

            for (int i = 0; i < steps && i < maxSteps; i++) {
                for (auto &pair: scene.getPool<RigidBodyComponent>()) {
                    auto &rb = *rigidBodies.at(pair.entity).get();
                    rb.applyForce(pair.component.force, pair.component.forcePoint / scale);
                    rb.applyTorque(pair.component.torque);
                    rb.applyLinearImpulse(pair.component.impulse, pair.component.impulsePoint);
                    rb.applyAngularImpulse(pair.component.angularImpulse);
                }

                world->step(DeltaTime(timeStep));
            }
        }
        for (auto &pair: scene.getPool<RigidBodyComponent>()) {
            auto &rb = *rigidBodies.at(pair.entity).get();
            auto transformComponent = scene.getComponent<TransformComponent>(pair.entity);

            transformComponent.transform.setPosition(rb.getPosition() * scale);
            transformComponent.transform.setRotation(Quaternion(rb.getRotation()));

            RigidBodyComponent rigidbodyComponent = pair.component;
            rigidbodyComponent.force = Vec3f();
            rigidbodyComponent.torque = Vec3f();
            rigidbodyComponent.impulse = Vec3f();
            rigidbodyComponent.angularImpulse = Vec3f();
            rigidbodyComponent.velocity = rb.getVelocity();
            rigidbodyComponent.angularVelocity = rb.getAngularVelocity();
            rigidbodyComponent.mass = rb.getMass();

            scene.updateComponent(pair.entity, rigidbodyComponent);
            scene.updateComponent(pair.entity, transformComponent);
        }
    }

    void PhysicsSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
    }

    void PhysicsSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == RigidBodyComponent::typeName) {
            if (colliders.find(entity) != colliders.end()) {
                for (auto &col: colliders.at(entity)) {
                    colliderIndices.erase(col.get());
                }
                colliders.erase(entity);
            }
            if (rigidBodies.find(entity) != rigidBodies.end()) {
                if (rigidBodies.at(entity)->hasFixedCollider()) {
                    colliderIndices.erase(&rigidBodies.at(entity)->getFixedCollider());
                }
                rigidBodiesReverse.erase(rigidBodies.at(entity).get());
                rigidBodies.erase(entity);
            }
        }
    }

    void PhysicsSystem::onComponentUpdate(const EntityHandle &entity,
                                          const Component &oldComponent,
                                          const Component &newComponent) {
        if (oldComponent.getTypeName() == RigidBodyComponent::typeName) {
            auto &oComp = down_cast<const RigidBodyComponent &>(oldComponent);
            auto &nComp = down_cast<const RigidBodyComponent &>(newComponent);

            if (rigidBodies.find(entity) != rigidBodies.end()) {
                rigidBodies.at(entity)->setRigidBodyType(nComp.type);
                rigidBodies.at(entity)->setAngularFactor(nComp.angularFactor);
                rigidBodies.at(entity)->setGravityScale(nComp.gravityScale);
            }
        } else if (oldComponent.getTypeName() == ColliderComponent::typeName) {
            auto &oComp = down_cast<const ColliderComponent &>(oldComponent);
            auto &nComp = down_cast<const ColliderComponent &>(newComponent);

            if (oComp.colliders != nComp.colliders && colliders.find(entity) != colliders.end()) {
                for (auto &col: colliders.at(entity)) {
                    colliderIndices.erase(col.get());
                }
                colliders.erase(entity);

                for (auto i = 0; i < nComp.colliders.size(); i++) {
                    auto collider = rigidBodies.at(entity)->createCollider(applyScale(
                        nComp.colliders.at(i), scale));
                    colliderIndices[collider.get()] = i;
                    colliders[entity].emplace_back(std::move(collider));
                }
            }
        } else if (oldComponent.getTypeName() == MeshColliderComponent::typeName) {
            // Recreate rigidbody
            if (rigidBodies.find(entity) != rigidBodies.end()) {
                rigidBodiesReverse.erase(rigidBodies.at(entity).get());
                rigidBodies.erase(entity);
            }
        }
    }

    void PhysicsSystem::beginContact(const World::Contact &contact) {
        auto entA = rigidBodiesReverse.at(&contact.colliderA.get().getBody());
        auto entB = rigidBodiesReverse.at(&contact.colliderB.get().getBody());
        auto indexA = colliderIndices.at(&contact.colliderA.get());
        auto indexB = colliderIndices.at(&contact.colliderB.get());

        if (bus != nullptr)
            bus->invoke(ContactEvent(ContactEvent::BEGIN_CONTACT,
                                     entA,
                                     entB,
                                     indexA,
                                     indexB));
    }

    void PhysicsSystem::endContact(const World::Contact &contact) {
        auto entA = rigidBodiesReverse.at(&contact.colliderA.get().getBody());
        auto entB = rigidBodiesReverse.at(&contact.colliderB.get().getBody());
        auto indexA = colliderIndices.at(&contact.colliderA.get());
        auto indexB = colliderIndices.at(&contact.colliderB.get());

        if (bus != nullptr)
            bus->invoke(ContactEvent(ContactEvent::END_CONTACT,
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
        auto it = rigidBodies.find(entity);
        if (it != rigidBodies.end()) {
            auto ptr = it->second.get();
            rigidBodies.erase(entity);
            rigidBodiesReverse.erase(ptr);
        }
    }
}
