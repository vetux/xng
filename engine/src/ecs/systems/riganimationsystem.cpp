/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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
#include "xng/ecs/components/riganimationcomponent.hpp"
#include "xng/ecs/components/skinnedmeshcomponent.hpp"
#include "xng/types/time.hpp"

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
            if (scene.checkComponent<SkinnedMeshComponent>(c.first)) {
                if (rigAnimators.find(c.first) == rigAnimators.end()) {
                    auto &meshComponent = scene.getComponent<SkinnedMeshComponent>(c.first);
                    rigAnimators[c.first] = RigAnimator(meshComponent.mesh.get().rig);
                    for (auto &pair: c.second.channels) {
                        rigAnimators.at(c.first).start(pair.second.animation.get(),
                                                       pair.second.blendDuration,
                                                       pair.second.loop,
                                                       pair.first);
                    }
                }
                rigAnimators.at(c.first).update(deltaTime);
                cUpdates[c.first] = c.second;
                cUpdates.at(c.first).boneTransforms = rigAnimators.at(c.first).getBoneTransforms();
            }
        }

        for (auto &pair: cUpdates) {
            scene.updateComponent(pair.first, pair.second);
        }
    }

    void RigAnimationSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(SkinnedMeshComponent)
            || component.getType() == typeid(RigAnimationComponent)) {
            rigAnimators.erase(entity);
        }
    }

    void RigAnimationSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getType() == typeid(SkinnedMeshComponent)
            || component.getType() == typeid(RigAnimationComponent)) {
            rigAnimators.erase(entity);
        }
    }

    void RigAnimationSystem::onComponentUpdate(const EntityHandle &entity,
                                               const Component &oldComponent,
                                               const Component &newComponent) {
        if (oldComponent.getType() == typeid(SkinnedMeshComponent)) {
            rigAnimators.erase(entity);
        } else if (oldComponent.getType() == typeid(RigAnimationComponent)) {
            auto &oc = dynamic_cast<const RigAnimationComponent &>(oldComponent);
            auto &nc = dynamic_cast<const RigAnimationComponent &>(newComponent);
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