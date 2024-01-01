/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_SPRITEANIMATIONSYSTEM_HPP
#define XENGINE_SPRITEANIMATIONSYSTEM_HPP

#include "xng/ecs/system.hpp"

#include "xng/ecs/components/spriteanimationcomponent.hpp"
#include "xng/util/time.hpp"

namespace xng {
    class XENGINE_EXPORT SpriteAnimationSystem : public System, public EntityScene::Listener {
    public:
        ~SpriteAnimationSystem() override = default;

        void start(EntityScene &scene, EventBus &eventBus) override;

        void stop(EntityScene &scene, EventBus &eventBus) override;

        void update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) override;

        std::string getName() override { return "SpriteAnimationSystem"; }

        void onComponentCreate(const EntityHandle &entity, const Component &component) override;

        void onComponentDestroy(const EntityHandle &entity, const Component &component) override;

        void onComponentUpdate(const EntityHandle &entity,
                               const Component &oldComponent,
                               const Component &newComponent) override;

        void onEntityDestroy(const EntityHandle &entity) override;

    private:
        std::map<EntityHandle, SpriteAnimation> animations;
    };
}
#endif //XENGINE_SPRITEANIMATIONSYSTEM_HPP
