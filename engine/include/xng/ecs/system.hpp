/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_SYSTEM_HPP
#define XENGINE_SYSTEM_HPP

#include "xng/util/time.hpp"
#include "xng/ecs/entityscene.hpp"

#include "xng/event/eventbus.hpp"
#include "xng/util/time.hpp"

namespace xng {
    class XENGINE_EXPORT System {
    public:
        virtual ~System() = default;

        // Bug: If these methods are declared pure virtual there are undefined references to the methods when linking to the resulting binary.
        virtual void start(EntityScene &scene, EventBus &eventBus) {}

        virtual void stop(EntityScene &scene, EventBus &eventBus) {}

        virtual void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {}

        virtual std::string getName() { return "System"; }
    };
}
#endif //XENGINE_SYSTEM_HPP
