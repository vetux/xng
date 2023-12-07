/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_WORLDBT3_HPP
#define XENGINE_WORLDBT3_HPP

#include <unordered_set>

#include "xng/physics/world.hpp"

#include "physics/bullet3/rigidbodybt3.hpp"
#include "physics/bullet3/colliderbt3.hpp"
#include "physics/bullet3/jointbt3.hpp"

#include "btBulletDynamicsCommon.h"

namespace xng {
    class WorldBt3 : public World {
    public:
        WorldBt3() {
            ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
            auto *collisionConfiguration = new btDefaultCollisionConfiguration();

            ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
            auto *dispatcher = new btCollisionDispatcher(collisionConfiguration);

            ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
            btBroadphaseInterface *overlappingPairCache = new btDbvtBroadphase();

            ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
            auto *solver = new btSequentialImpulseConstraintSolver;

            dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver,
                                                        collisionConfiguration);
        }

        std::unique_ptr<RigidBody> createBody() override {
            return std::make_unique<RigidBodyBt3>(dynamicsWorld);
        }

        std::unique_ptr<RigidBody> createBody(const ColliderDesc &colliderDesc, RigidBody::RigidBodyType type) override {
            auto ret = std::make_unique<RigidBodyBt3>(dynamicsWorld, colliderDesc, type);
            colliders[ret.get()->body] = ColliderBt3(*ret);
            return ret;
        }

        std::unique_ptr<Joint> createJoint() override {
            return std::make_unique<JointBt3>();
        }

        void addContactListener(ContactListener &listener) override {
            listeners.insert(&listener);
        }

        void removeContactListener(ContactListener &listener) override {
            listeners.erase(&listener);
        }

        void setGravity(const Vec3f &gravity) override {
            dynamicsWorld->setGravity(convert(gravity));
        }

        void step(float deltaTime) override {
            dynamicsWorld->stepSimulation(deltaTime, -1);

            std::unordered_set<Contact> contacts;

            int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
            for (int i = 0; i < numManifolds; i++) {
                btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
                const auto *obA = static_cast<const btCollisionObject *>(contactManifold->getBody0());
                const auto *obB = static_cast<const btCollisionObject *>(contactManifold->getBody1());

                auto &colA = colliders.at(obA);
                auto &colB = colliders.at(obB);

                int numContacts = contactManifold->getNumContacts();
                for (int j = 0; j < numContacts; j++) {
                    btManifoldPoint &pt = contactManifold->getContactPoint(j);
                    if (pt.getDistance() < 0.f) {
                        const btVector3 &ptA = pt.getPositionWorldOnA();
                        const btVector3 &ptB = pt.getPositionWorldOnB();
                        const btVector3 &normalOnB = pt.m_normalWorldOnB;

                        Contact c(colliders.at(obA),
                                  colliders.at(obB),
                                  convert(ptA),
                                  convert(ptB),
                                  convert(normalOnB));
                        contacts.insert(c);
                        if (!existingContacts.contains(c)){
                            for(auto &l : listeners){
                                l->beginContact(c);
                            }
                        }
                        existingContacts.insert(c);
                    }
                }
            }

            std::unordered_set<Contact> delContacts;

            for (auto & c: existingContacts){
                if (contacts.find(c) == contacts.end()){
                    for(auto &l : listeners){
                        l->endContact(c);
                    }
                    delContacts.insert(c);
                }
            }

            for(auto &d : delContacts){
                existingContacts.erase(d);
            }
        }

        void step(float d, int maxSteps) override {
            dynamicsWorld->stepSimulation(d, maxSteps);

            std::unordered_set<Contact> contacts;

            int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
            for (int i = 0; i < numManifolds; i++) {
                btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
                const auto *obA = static_cast<const btCollisionObject *>(contactManifold->getBody0());
                const auto *obB = static_cast<const btCollisionObject *>(contactManifold->getBody1());

                auto &colA = colliders.at(obA);
                auto &colB = colliders.at(obB);

                int numContacts = contactManifold->getNumContacts();
                for (int j = 0; j < numContacts; j++) {
                    btManifoldPoint &pt = contactManifold->getContactPoint(j);
                    if (pt.getDistance() < 0.f) {
                        const btVector3 &ptA = pt.getPositionWorldOnA();
                        const btVector3 &ptB = pt.getPositionWorldOnB();
                        const btVector3 &normalOnB = pt.m_normalWorldOnB;

                        Contact c(colliders.at(obA),
                                  colliders.at(obB),
                                  convert(ptA),
                                  convert(ptB),
                                  convert(normalOnB));
                        contacts.insert(c);
                        if (!existingContacts.contains(c)){
                            for(auto &l : listeners){
                                l->beginContact(c);
                            }
                        }
                        existingContacts.insert(c);
                    }
                }
            }

            std::unordered_set<Contact> delContacts;

            for (auto & c: existingContacts){
                if (contacts.find(c) == contacts.end()){
                    for(auto &l : listeners){
                        l->endContact(c);
                    }
                    delContacts.insert(c);
                }
            }

            for(auto &d : delContacts){
                existingContacts.erase(d);
            }
        }

    private:
        btDiscreteDynamicsWorld *dynamicsWorld;

        std::set<ContactListener *> listeners;

        std::unordered_set<Contact> existingContacts;

        std::map<const btCollisionObject*, ColliderBt3> colliders;

    };
}


#endif //XENGINE_WORLDBT3_HPP
