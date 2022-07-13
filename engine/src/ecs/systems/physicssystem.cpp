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

#include "ecs/systems/physicssystem.hpp"

#include "ecs/components.hpp"

namespace xng {
    PhysicsSystem::PhysicsSystem(World &world)
            : world(world) {}

    void PhysicsSystem::start(EntityScene &entityManager) {
        System::start(entityManager);
    }

    void PhysicsSystem::stop(EntityScene &entityManager) {
        System::stop(entityManager);
    }

    void PhysicsSystem::update(DeltaTime deltaTime, EntityScene &entityManager) {
        System::update(deltaTime, entityManager);
    }
}