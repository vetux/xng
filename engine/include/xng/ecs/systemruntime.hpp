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

#ifndef XENGINE_SYSTEMRUNTIME_HPP
#define XENGINE_SYSTEMRUNTIME_HPP

#include <set>
#include <memory>

#include "xng/ecs/system.hpp"
#include "xng/ecs/entity.hpp"
#include "xng/ecs/systempipeline.hpp"
#include "xng/ecs/profiling/ecsprofiler.hpp"

#include "xng/event/eventbus.hpp"

#include "xng/types/deltatime.hpp"

namespace xng {
    class XENGINE_EXPORT SystemRuntime {
    public:
        explicit SystemRuntime(std::vector<SystemPipeline> pipelines = {},
                               std::shared_ptr<EntityScene> scene = std::make_shared<EntityScene>(),
                               std::shared_ptr<EventBus> eventBus = std::make_shared<EventBus>());

        ~SystemRuntime();

        SystemRuntime(SystemRuntime &&other) noexcept;

        SystemRuntime &operator=(SystemRuntime &&other) noexcept;

        void start();

        void update(DeltaTime deltaTime);

        void stop();

        const std::shared_ptr<EntityScene> &getScene() const;

        void setScene(const std::shared_ptr<EntityScene> &scene);

        const std::vector<SystemPipeline> &getPipelines() const;

        void setPipelines(const std::vector<SystemPipeline> &systems);

        void setEnableProfiling(bool value) {
            enableProfiling = value;
        }

        const ECSProfiler &getProfiler() const;

        void setEventBus(const std::shared_ptr<EventBus> &ptr) {
            bool reset = started;
            if (started) {
                stop();
            }
            if (ptr) {
                eventBus = ptr;
            } else {
                eventBus = std::make_shared<EventBus>();
            }
            if (reset) {
                start();
            }
        }

        std::shared_ptr<EventBus> getEventBus() const {
            return eventBus;
        }

    private:
        bool enableProfiling = false;
        ECSProfiler profiler;
        bool started = false;
        std::shared_ptr<EntityScene> scene;
        std::vector<SystemPipeline> pipelines;
        std::shared_ptr<EventBus> eventBus;
    };
}

#endif //XENGINE_SYSTEMRUNTIME_HPP
