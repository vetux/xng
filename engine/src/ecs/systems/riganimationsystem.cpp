/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "xng/ecs/systems/riganimationsystem.hpp"
#include "xng/ecs/components/rendering/riganimationcomponent.hpp"
#include "xng/ecs/components/rendering/skinnedmodelcomponent.hpp"
#include "xng/util/time.hpp"

namespace xng {
    void RigAnimationSystem::start(EntityScene &scene, EventBus &eventBus) {
        scene.addListener(*this);
    }

    void RigAnimationSystem::stop(EntityScene &scene, EventBus &eventBus) {
        scene.removeListener(*this);
    }

    void RigAnimationSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        std::map<EntityHandle, RigAnimationComponent> cUpdates;
        for (auto &c: scene.getPool<RigAnimationComponent>()) {
            if (scene.checkComponent<SkinnedModelComponent>(c.entity)) {
                if (rigAnimators.find(c.entity) == rigAnimators.end()) {
                    auto &meshComponent = scene.getComponent<SkinnedModelComponent>(c.entity);
                    rigAnimators[c.entity] = RigAnimator(meshComponent.model.get().rig);
                    for (auto &pair: c.component.channels) {
                        rigAnimators.at(c.entity).start(pair.second.animation.get(),
                                                        pair.second.blendDuration,
                                                        pair.second.loop,
                                                        pair.first);
                    }
                }
                rigAnimators.at(c.entity).update(deltaTime);
                cUpdates[c.entity] = c.component;
                cUpdates.at(c.entity).boneTransforms = rigAnimators.at(c.entity).getBoneTransforms();
            }
        }

        for (auto &pair: cUpdates) {
            scene.updateComponent(pair.first, pair.second);
        }
    }

    void RigAnimationSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == SkinnedModelComponent::typeName
            || component.getTypeName() == RigAnimationComponent::typeName) {
            rigAnimators.erase(entity);
        }
    }

    void RigAnimationSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == SkinnedModelComponent::typeName
            || component.getTypeName() == RigAnimationComponent::typeName) {
            rigAnimators.erase(entity);
        }
    }

    void RigAnimationSystem::onComponentUpdate(const EntityHandle &entity,
                                               const Component &oldComponent,
                                               const Component &newComponent) {
        if (oldComponent.getTypeName() == SkinnedModelComponent::typeName) {
            rigAnimators.erase(entity);
        } else if (oldComponent.getTypeName() == RigAnimationComponent::typeName) {
            auto &oc = down_cast<const RigAnimationComponent &>(oldComponent);
            auto &nc = down_cast<const RigAnimationComponent &>(newComponent);
            for (auto &pair: oc.channels) {
                if (nc.channels.find(pair.first) == nc.channels.end()) {
                    rigAnimators.at(entity).stop(pair.first);
                }
            }
            for (auto &pair: nc.channels) {
                auto it = oc.channels.find(pair.first);
                bool skipUpdate = false;
                if (it != oc.channels.end()) {
                    if (pair.second == it->second) {
                        skipUpdate = true;
                    }
                }
                if (!skipUpdate) {
                    rigAnimators.at(entity).start(pair.second.animation.get(),
                                                  pair.second.blendDuration,
                                                  pair.second.loop,
                                                  pair.first);
                }
            }
        }
    }

    void RigAnimationSystem::onEntityDestroy(const EntityHandle &entity) {
        rigAnimators.erase(entity);
    }
}
