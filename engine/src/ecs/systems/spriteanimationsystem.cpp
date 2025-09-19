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

#include "xng/ecs/systems/spriteanimationsystem.hpp"

#include "xng/ecs/components/rendering/spritecomponent.hpp"
#include "xng/util/time.hpp"

namespace xng {
    void SpriteAnimationSystem::start(EntityScene &scene, EventBus &eventBus) {
        scene.addListener(*this);
        for (auto &pair: scene.getPool<SpriteAnimationComponent>()) {
            if (animations.find(pair.entity) == animations.end()) {
                if (pair.component.animation.assigned())
                    animations[pair.entity] = pair.component.animation.get();
            }
        }
    }

    void SpriteAnimationSystem::stop(EntityScene &scene, EventBus &eventBus) {
        scene.removeListener(*this);
        animations.clear();
    }

    void SpriteAnimationSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        for (const auto &c: scene.getPool<SpriteAnimationComponent>()) {
            if (!c.component.enabled)
                continue;
            if (c.component.animation.assigned()) {
                // Advance animation
                auto &anim = animations.at(c.entity);
                anim.setAnimationSpeed(c.component.animationSpeed);

                auto &f = anim.getFrame(deltaTime);
                if (anim.getTime() == anim.getDuration() && !anim.isLooping()) {
                    SpriteAnimationComponent comp = c.component;
                    comp.finished = true;
                    scene.updateComponent(c.entity, comp);
                }
                // Update sprite
                if (scene.checkComponent<SpriteComponent>(c.entity)) {
                    auto ren = scene.getComponent<SpriteComponent>(c.entity);
                    ren.sprite = f;
                    scene.updateComponent(c.entity, ren);
                }
            }
        }
    }

    void SpriteAnimationSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == SpriteAnimationComponent::typeName) {
            const auto &v = down_cast<const SpriteAnimationComponent &>(component);
            if (v.animation.assigned()) {
                auto animation = v.animation.get();
                animations[entity] = animation;
            }
        }
    }

    void SpriteAnimationSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == SpriteAnimationComponent::typeName) {
            animations.erase(entity);
        }
    }

    void SpriteAnimationSystem::onEntityDestroy(const EntityHandle &entity) {
        animations.erase(entity);
    }

    void SpriteAnimationSystem::onComponentUpdate(const EntityHandle &entity,
                                                  const Component &oldComponent,
                                                  const Component &newComponent) {
        if (oldComponent.getTypeName() == SpriteAnimationComponent::typeName) {
            const auto &ov = down_cast<const SpriteAnimationComponent &>(oldComponent);
            const auto &nv = down_cast<const SpriteAnimationComponent &>(newComponent);
            if (ov.animation != nv.animation) {
                if (nv.animation.assigned())
                    animations[entity] = nv.animation.get();
                else
                    animations.erase(entity);
            }
        }
    }
}