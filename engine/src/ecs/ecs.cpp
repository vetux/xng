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

#include "ecs/ecs.hpp"

#include <utility>
#include <algorithm>

namespace xng {
    ECS::ECS(std::vector<std::reference_wrapper<System>> systems, std::shared_ptr<EntityScene> scene)
            : systems(std::move(systems)), scene(std::move(scene)) {}

    ECS::~ECS() = default;

    ECS::ECS(ECS &&other) noexcept = default;

    ECS &ECS::operator=(ECS &&other) noexcept = default;

    void ECS::start() {
        if (!scene) {
            throw std::runtime_error("No scene assigned.");
        }

        for (auto &system: systems) {
            system.get().start(*scene);
        }
        started = true;
    }

    void ECS::update(DeltaTime deltaTime) {
        for (auto &system: systems) {
            system.get().update(deltaTime, *scene);
        }
    }

    void ECS::stop() {
        for (auto &system: systems) {
            system.get().stop(*scene);
        }
        started = false;
    }

    const std::shared_ptr<EntityScene> &ECS::getScene() const {
        return scene;
    }

    void ECS::setScene(const std::shared_ptr<EntityScene> &v) {
        if (started)
            stop();
        scene = v;
    }

    const std::vector<std::reference_wrapper<System>> &ECS::getSystems() const {
        return systems;
    }

    void ECS::setSystems(const std::vector<std::reference_wrapper<System>> &v) {
        stop();
        systems = v;
    }
}