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

#include "xng/ecs/systems/spriteanimationsystem.hpp"

#include "xng/ecs/components/spritecomponent.hpp"
#include "xng/types/time.hpp"

namespace xng {
    void SpriteAnimationSystem::start(EntityScene &scene, EventBus &eventBus) {
        scene.addListener(*this);
        for (auto &pair: scene.getPool<SpriteAnimationComponent>()) {
            if (animations.find(pair.first) == animations.end()) {
                if (pair.second.animation.assigned())
                    animations[pair.first] = pair.second.animation.get();
            }
        }
    }

    void SpriteAnimationSystem::stop(EntityScene &scene, EventBus &eventBus) {
        scene.removeListener(*this);
        animations.clear();
    }

    void SpriteAnimationSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        for (const auto &c: scene.getPool<SpriteAnimationComponent>()) {
            if (!c.second.enabled)
                continue;
            if (c.second.animation.assigned()) {
                // Advance animation
                auto &anim = animations.at(c.first);
                if (c.second.animationDurationOverride > 0) {
                    anim.setDuration(c.second.animationDurationOverride);
                } else {
                    anim.setDuration(c.second.animation.get().getDuration());
                }
                auto &f = anim.getFrame(deltaTime);
                if (anim.getTime() == anim.getDuration() && !anim.isLooping()) {
                    SpriteAnimationComponent comp = c.second;
                    comp.finished = true;
                    scene.updateComponent(c.first, comp);
                }
                // Update sprite
                if (scene.checkComponent<SpriteComponent>(c.first)) {
                    auto ren = scene.getComponent<SpriteComponent>(c.first);
                    ren.sprite = f;
                    scene.updateComponent(c.first, ren);
                }
            }
        }
    }

    void SpriteAnimationSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(SpriteAnimationComponent)) {
            const auto &v = dynamic_cast<const SpriteAnimationComponent &>(component);
            if (v.animation.assigned()) {
                auto animation = v.animation.get();
                animations[entity] = animation;
            }
        }
    }

    void SpriteAnimationSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(SpriteAnimationComponent)) {
            animations.erase(entity);
        }
    }

    void SpriteAnimationSystem::onEntityDestroy(const EntityHandle &entity) {
        animations.erase(entity);
    }

    void SpriteAnimationSystem::onComponentUpdate(const EntityHandle &entity,
                                                  const Component &oldComponent,
                                                  const Component &newComponent) {
        if (oldComponent.getType() == typeid(SpriteAnimationComponent)) {
            const auto &ov = dynamic_cast<const SpriteAnimationComponent &>(oldComponent);
            const auto &nv = dynamic_cast<const SpriteAnimationComponent &>(newComponent);
            if (ov.animation != nv.animation) {
                if (nv.animation.assigned())
                    animations[entity] = nv.animation.get();
                else
                    animations.erase(entity);
            }
        }
    }
}