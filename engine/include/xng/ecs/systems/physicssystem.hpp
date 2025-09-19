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

#ifndef XENGINE_PHYISCS3DSYSTEM_HPP
#define XENGINE_PHYISCS3DSYSTEM_HPP

#include "xng/ecs/system.hpp"
#include "xng/event/eventbus.hpp"
#include "xng/util/time.hpp"
#include "xng/physics/physicsengine.hpp"

namespace xng {
    class XENGINE_EXPORT PhysicsSystem final : public System, public EntityScene::Listener, public World::ContactListener {
    public:
        PhysicsSystem(std::shared_ptr<PhysicsEngine> physicsEngine, float scale, float timeStep);

        PhysicsSystem(std::shared_ptr<PhysicsEngine> physicsEngine, float scale, int maxSteps);

        ~PhysicsSystem() override = default;

        PhysicsSystem(const PhysicsSystem &other) = delete;

        PhysicsSystem & operator=(const PhysicsSystem &other) = delete;

        void start(EntityScene &entityManager, EventBus &eventBus) override;

        void stop(EntityScene &entityManager, EventBus &eventBus) override;

        void update(DeltaTime deltaTime, EntityScene &entityManager, EventBus &eventBus) override;

        std::string getName() override { return "PhysicsSystem"; }

        void onComponentCreate(const EntityHandle &entity, const Component &component) override;

        void onComponentDestroy(const EntityHandle &entity, const Component &component) override;

        void onComponentUpdate(const EntityHandle &entity,
                               const Component &oldComponent,
                               const Component &newComponent) override;

        void onEntityDestroy(const EntityHandle &entity) override;

        void beginContact(const World::Contact &contact) override;

        void endContact(const World::Contact &contact) override;

    private:
        EventBus *bus = nullptr;

        std::shared_ptr<PhysicsEngine> engine;

        std::unique_ptr<World> world;

        std::map<EntityHandle, std::unique_ptr<RigidBody>> rigidBodies;
        std::map<RigidBody *, EntityHandle> rigidBodiesReverse;
        std::map<EntityHandle, std::vector<std::unique_ptr<Collider>>> colliders;
        std::map<Collider *, size_t> colliderIndices;

        float scale = 20; // The number of units which correspond to a metre in the physics world.
        float timeStep = 0; // The duration of one physics world step
        float deltaAccumulator = 0;

        int maxSteps = 10;
    };
}

#endif //XENGINE_PHYISCS3DSYSTEM_HPP
