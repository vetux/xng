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

#include "xng/ecs/systemruntime.hpp"
#include "xng/util/time.hpp"

#include <utility>
#include <algorithm>

namespace xng {
    SystemRuntime::SystemRuntime(std::vector<SystemPipeline> systems,
                                 std::shared_ptr<EntityScene> scene,
                                 std::shared_ptr<EventBus> eventBus)
            : pipelines(std::move(systems)),
              scene(std::move(scene)),
              eventBus(std::move(eventBus)) {}

    SystemRuntime::~SystemRuntime() = default;

    SystemRuntime::SystemRuntime(SystemRuntime &&other) noexcept = default;

    SystemRuntime &SystemRuntime::operator=(SystemRuntime &&other) noexcept = default;

    void SystemRuntime::start() {
        if (!scene) {
            throw std::runtime_error("No scene assigned.");
        }

        for (auto &pipeline: pipelines) {
            pipeline.start(*scene, *eventBus);
        }
        started = true;
    }

    void SystemRuntime::update(DeltaTime deltaTime) {
        if (enableProfiling) {
            profiler.beginFrame();
            for (auto &pipeline: pipelines) {
                profiler.beginPipelineUpdate();
                pipeline.update(deltaTime, *scene, *eventBus, profiler, enableProfiling);
                profiler.endPipelineUpdate(pipeline.getName());
            }
            profiler.endFrame();
        } else {
            for (auto &pipeline: pipelines) {
                pipeline.update(deltaTime, *scene, *eventBus, profiler, enableProfiling);
            }
        }
    }

    void SystemRuntime::stop() {
        for (auto &pipeline: pipelines) {
            pipeline.stop(*scene, *eventBus);
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

    const std::vector<SystemPipeline> &SystemRuntime::getPipelines() const {
        return pipelines;
    }

    void SystemRuntime::setPipelines(const std::vector<SystemPipeline> &v) {
        if (started)
            stop();
        pipelines = v;
    }

    const ECSProfiler &SystemRuntime::getProfiler() const {
        return profiler;
    }
}