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

#include "ecs/systems/spriteanimationsystem.hpp"

#include "ecs/components/spritecomponent.hpp"

void SpriteAnimationSystem::start(EntityScene &scene) {
    scene.addListener(*this);
    for (auto &pair: scene.getPool<SpriteAnimationComponent>()) {
        if (animations.find(pair.first) == animations.end()) {
            if (pair.second.animation.assigned())
                animations[pair.first] = pair.second.animation.get();
        }
    }
}

void SpriteAnimationSystem::stop(EntityScene &scene) {
    scene.removeListener(*this);
    animations.clear();
}

void SpriteAnimationSystem::update(DeltaTime deltaTime, EntityScene &scene) {
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
            if (scene.check<SpriteComponent>(c.first)) {
                auto ren = scene.lookup<SpriteComponent>(c.first);
                ren.sprite = f;
                scene.updateComponent(c.first, ren);
            }
        }
    }
}

void SpriteAnimationSystem::onComponentCreate(const EntityHandle &entity, const std::any &component) {
    if (component.type() == typeid(SpriteAnimationComponent)) {
        const auto *v = std::any_cast<SpriteAnimationComponent>(&component);
        if (v->animation.assigned()) {
            auto animation = v->animation.get();
            animations[entity] = animation;
        }
    }
}

void SpriteAnimationSystem::onComponentDestroy(const EntityHandle &entity, const std::any &component) {
    if (component.type() == typeid(SpriteAnimationComponent)) {
        animations.erase(entity);
    }
}

void SpriteAnimationSystem::onComponentUpdate(const EntityHandle &entity,
                                              const std::any &oldComponent,
                                              const std::any &newComponent) {
    if (oldComponent.type() == typeid(SpriteAnimationComponent)) {
        const auto *ov = std::any_cast<SpriteAnimationComponent>(&oldComponent);
        const auto *nv = std::any_cast<SpriteAnimationComponent>(&newComponent);
        if (ov->animation != nv->animation) {
            if (nv->animation.assigned())
                animations[entity] = nv->animation.get();
            else
                animations.erase(entity);
        }
    }
}
