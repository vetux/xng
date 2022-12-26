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

#include "xng/ecs/systemruntime.hpp"

#include <utility>
#include <algorithm>

namespace xng {
    SystemRuntime::SystemRuntime(std::vector<std::reference_wrapper<System>> systems, std::shared_ptr<EntityScene> scene)
            : systems(std::move(systems)), scene(std::move(scene)) {}

    SystemRuntime::~SystemRuntime() = default;

    SystemRuntime::SystemRuntime(SystemRuntime &&other) noexcept = default;

    SystemRuntime &SystemRuntime::operator=(SystemRuntime &&other) noexcept = default;

    void SystemRuntime::start() {
        if (!scene) {
            throw std::runtime_error("No scene assigned.");
        }

        for (auto &system: systems) {
            system.get().start(*scene, *eventBus);
        }
        started = true;
    }

    void SystemRuntime::update(DeltaTime deltaTime) {
        if (enableProfiling) {
            profiler.beginFrame();
            for (auto &system: systems) {
                profiler.beginSystemUpdate();
                system.get().update(deltaTime, *scene, *eventBus);
                profiler.endSystemUpdate(system.get().getName());
            }
            profiler.endFrame();
        } else {
            for (auto &system: systems) {
                system.get().update(deltaTime, *scene, *eventBus);
            }
        }
    }

    void SystemRuntime::stop() {
        for (auto &system: systems) {
            system.get().stop(*scene,*eventBus);
        }
        started = false;
    }

    const std::shared_ptr<EntityScene> &SystemRuntime::getScene() const {
        return scene;
    }

    void SystemRuntime::setScene(const std::shared_ptr<EntityScene> &v) {
        auto restart = started;
        if (started)
            stop();

        scene = v;

        if (restart)
            start();
    }

    const std::vector<std::reference_wrapper<System>> &SystemRuntime::getSystems() const {
        return systems;
    }

    void SystemRuntime::setSystems(const std::vector<std::reference_wrapper<System>> &v) {
        if (started)
            stop();
        systems = v;
    }

    const ECSFrameList &SystemRuntime::getFrameList() const {
        return profiler.getFrames();
    }
}