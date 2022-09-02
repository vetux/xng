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

#ifndef XENGINE_PHYISCS3DSYSTEM_HPP
#define XENGINE_PHYISCS3DSYSTEM_HPP

#include "event/eventbus.hpp"
#include "ecs/system.hpp"

#include "physics/world.hpp"

namespace xng {
    class XENGINE_EXPORT PhysicsSystem : public System, public EntityScene::Listener, public World::ContactListener {
    public:
        PhysicsSystem(World &world, EventBus &eventBus, float scale);

        ~PhysicsSystem() override = default;

        void start(EntityScene &entityManager) override;

        void stop(EntityScene &entityManager) override;

        void update(DeltaTime deltaTime, EntityScene &entityManager) override;

        void onComponentCreate(const EntityHandle &entity, const std::any &component) override;

        void onComponentDestroy(const EntityHandle &entity, const std::any &component) override;

        void onComponentUpdate(const EntityHandle &entity, const std::any &oldComponent,
                               const std::any &newComponent) override;

        void onEntityDestroy(const EntityHandle &entity) override;

        void beginContact(World::Contact &contact) override;

        void endContact(World::Contact &contact) override;

    private:
        World &world;
        EventBus &eventBus;

        std::map<EntityHandle, std::unique_ptr<RigidBody>> rigidbodies;
        std::map<RigidBody *, EntityHandle> rigidbodiesReverse;
        std::map<EntityHandle, std::vector<std::unique_ptr<Collider>>> colliders;
        std::map<Collider *, size_t> colliderIndices;

        float scale = 20; // The number of units which correspond to a metre in the physics world.
    };
}

#endif //XENGINE_PHYISCS3DSYSTEM_HPP
