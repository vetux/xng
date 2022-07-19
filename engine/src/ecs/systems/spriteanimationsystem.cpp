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
}

void SpriteAnimationSystem::stop(EntityScene &scene) {
    scene.removeListener(*this);
}

void SpriteAnimationSystem::update(DeltaTime deltaTime, EntityScene &scene) {
    for (const auto &c: scene.getPool<SpriteAnimationComponent>()) {
        if (!c.second.enabled)
            continue;
        // Advance animation
        auto &f = animations.at(c.first).getFrame(deltaTime);
        // Update sprite
        if (scene.check<SpriteComponent>(c.first)) {
            auto ren = scene.lookup<SpriteComponent>(c.first);
            ren.sprite = f;
            scene.updateComponent(c.first, ren);
        }
    }
}

void SpriteAnimationSystem::onComponentCreate(const EntityHandle &entity,
                                              const std::any &component,
                                              std::type_index componentType) {
    if (componentType == typeid(SpriteAnimationComponent)) {
        const auto *v = std::any_cast<SpriteAnimationComponent>(&component);
        auto animation = v->animation.get();
        animations[entity] = animation;
    }
}

void SpriteAnimationSystem::onComponentDestroy(const EntityHandle &entity,
                                               const std::any &component,
                                               std::type_index componentType) {
    if (componentType == typeid(SpriteAnimationComponent)) {
        animations.erase(entity);
    }
}

void SpriteAnimationSystem::onComponentUpdate(const EntityHandle &entity,
                                              const std::any &oldComponent,
                                              const std::any &newComponent,
                                              std::type_index componentType) {
    if (componentType == typeid(SpriteAnimationComponent)) {
        const auto *ov = std::any_cast<SpriteAnimationComponent>(&oldComponent);
        const auto *nv = std::any_cast<SpriteAnimationComponent>(&newComponent);
        if (ov->animation != nv->animation) {
            animations[entity] = nv->animation.get();
        }
    }
}
