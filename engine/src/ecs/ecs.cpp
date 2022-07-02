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

#include "ecs/ecs.hpp"

#include <utility>
#include <algorithm>

namespace xng {
    ECS::ECS(std::vector<std::unique_ptr<System>> systems)
            : systems(std::move(systems)) {}

    ECS::~ECS() = default;

    ECS::ECS(ECS &&other) noexcept = default;

    ECS &ECS::operator=(ECS &&other) noexcept = default;

    void ECS::start() {
        for (auto &system: systems) {
            system->start(entityManager);
        }
    }

    void ECS::update(float deltaTime) {
        for (auto &system: systems) {
            system->update(deltaTime, entityManager);
        }
    }

    void ECS::stop() {
        for (auto &system: systems) {
            system->stop(entityManager);
        }
    }

    EntityContainer &ECS::getEntityContainer() {
        return entityManager;
    }
}