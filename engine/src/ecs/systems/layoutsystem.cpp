/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/ecs/systems/layoutsystem.hpp"

namespace xng {
    LayoutSystem::LayoutSystem() {
    }

    void LayoutSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        System::update(deltaTime, scene, eventBus);
    }

    void LayoutSystem::start(EntityScene &scene, EventBus &eventBus) {
        System::start(scene, eventBus);
    }

    void LayoutSystem::stop(EntityScene &scene, EventBus &eventBus) {
        System::stop(scene, eventBus);
    }
}
