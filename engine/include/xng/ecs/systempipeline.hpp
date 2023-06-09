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

#ifndef XENGINE_SYSTEMPIPELINE_HPP
#define XENGINE_SYSTEMPIPELINE_HPP

#include <memory>
#include <vector>
#include <chrono>

#include "xng/ecs/system.hpp"
#include "xng/ecs/profiling/ecsprofiler.hpp"
#include "xng/async/threadpool.hpp"

namespace xng {
    /**
     * A system pipeline represents a list of systems that are invoked using the specified tick mode.
     *
     * Pipelines are executed sequentially by the SystemRuntime.
     *
     * If runAsync is set the system updates are executed asynchronously.
     * In that case the user must handle synchronization between the executing systems if required.
     */
    class SystemPipeline {
    public:
        enum TickMode {
            TICK_FRAME, // Invoke the system updates once every frame passing the duration of the duration of the last frame as deltaTime.
            TICK_CLAMP, // Invoke the system updates at an interval less than or equal to fixedStepDuration. Good for physics systems for example.
            TICK_FIXED, // Invoke the system updates at a fixed interval specified by fixedStepDuration, The remaining time in a frame is stored and used in the next frame causing stuttering. Therefore this should only be used for non real time updates. The advantage is that the passed deltaTime is always the value of fixedStepDuration.
        };

        /**
         * @param tickMode
         * @param fixedStepDuration
         * @param fixedStepMaxSteps Specify the maximum number of steps to take in TICK_CLAMP and TICK_FIXED modes preventing the "Spiral of death" at the cost of stuttering of clamped/fixed ticks.
         * @param runAsync
         * @param systems
         */
        explicit SystemPipeline(TickMode tickMode,
                                std::vector<std::shared_ptr<System>> systems = {},
                                DeltaTime fixedStepDuration = 0,
                                int fixedStepMaxSteps = 0,
                                bool runAsync = false,
                                std::string name = "Unnamed Pipeline")
                : tickMode(tickMode),
                  fixedStepDuration(fixedStepDuration),
                  fixedStepMaxSteps(fixedStepMaxSteps),
                  runAsync(runAsync),
                  systems(std::move(systems)),
                  name(std::move(name)) {}

        /**
         * Create a TICK_FRAME pipeline
         *
         * @param systems
         * @param name
         */
        explicit SystemPipeline(std::vector<std::shared_ptr<System>> systems = {},
                                bool runAsync = false,
                                std::string name = "Unnamed Pipeline")
                : tickMode(TICK_FRAME),
                  runAsync(runAsync),
                  systems(std::move(systems)),
                  name(std::move(name)) {}

        void addSystem(const std::shared_ptr<System> &ptr) {
            systems.emplace_back(ptr);
        }

        void start(EntityScene &scene, EventBus &eventBus) {
            for (auto &ptr: systems) {
                ptr->start(scene, eventBus);
            }
        }

        void stop(EntityScene &scene, EventBus &eventBus) {
            for (auto &ptr: systems) {
                ptr->stop(scene, eventBus);
            }
        }

        /**
         * Return when all the system updates have completed,
         * optionally scheduling the updates on the global thread pool.
         */
        void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus, ECSProfiler &profiler,
                    bool enableProfiling) {
            switch (tickMode) {
                case TICK_FRAME: {
                    invokeUpdate(deltaTime, scene, eventBus, profiler, enableProfiling);
                    break;
                }
                case TICK_FIXED: {
                    fixedStepAccumulator += deltaTime;
                    int steps = static_cast<int>(fixedStepAccumulator / fixedStepDuration);
                    steps = std::clamp(steps, 0, fixedStepMaxSteps);
                    fixedStepAccumulator -= fixedStepDuration * static_cast<float>(steps);
                    for (int i = 0; i < steps; i++) {
                        invokeUpdate(fixedStepDuration, scene, eventBus, profiler, enableProfiling);
                    }
                    break;
                }
                case TICK_CLAMP: {
                    fixedStepAccumulator += deltaTime;
                    int steps = static_cast<int>(fixedStepAccumulator / fixedStepDuration);
                    steps = std::clamp(steps, 0, fixedStepMaxSteps);
                    fixedStepAccumulator -= fixedStepDuration * static_cast<float>(steps);
                    for (int i = 0; i < steps; i++) {
                        invokeUpdate(fixedStepDuration, scene, eventBus, profiler, enableProfiling);
                    }
                    invokeUpdate(fixedStepAccumulator, scene, eventBus, profiler, enableProfiling);
                    fixedStepAccumulator = 0;
                    break;
                }
            }
        }

        const std::string &getName() {
            return name;
        }

    private:
        TickMode tickMode;
        DeltaTime fixedStepDuration;
        int fixedStepMaxSteps;
        bool runAsync;
        std::vector<std::shared_ptr<System>> systems;
        std::string name;

        DeltaTime fixedStepAccumulator = 0;

        void invokeUpdate(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus, ECSProfiler &profiler,
                          bool enableProfiling) {
            if (runAsync) {
                std::vector<std::shared_ptr<Task>> tasks;
                for (auto &ptr: systems) {
                    tasks.emplace_back(ThreadPool::getPool().addTask(
                            [ptr, deltaTime, &scene, &eventBus, &profiler, enableProfiling]() {
                                if (enableProfiling) {
                                    int id = profiler.beginSystemUpdate();
                                    ptr->update(deltaTime, scene, eventBus);
                                    profiler.endSystemUpdate(ptr->getName(), id);
                                } else {
                                    ptr->update(deltaTime, scene, eventBus);
                                }
                            }));
                }
                for (auto &task: tasks) {
                    task->join();
                }
            } else {
                for (auto &ptr: systems) {
                    if (enableProfiling) {
                        int id = profiler.beginSystemUpdate();
                        ptr->update(deltaTime, scene, eventBus);
                        profiler.endSystemUpdate(ptr->getName(), id);
                    } else {
                        ptr->update(deltaTime, scene, eventBus);
                    }
                }
            }
        }
    };
}

#endif //XENGINE_SYSTEMPIPELINE_HPP
