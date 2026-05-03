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

#include "xng/ecs/systems/nodeanimationsystem.hpp"
#include "xng/ecs/components/rendering/skinnedmodelcomponent.hpp"
#include "xng/util/time.hpp"

namespace xng {
    void NodeAnimationSystem::start(EntityScene &scene, EventBus &eventBus) {
        scene.addListener(*this);
    }

    void NodeAnimationSystem::stop(EntityScene &scene, EventBus &eventBus) {
        scene.removeListener(*this);
    }

    void NodeAnimationSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        std::map<EntityHandle, NodeAnimationComponent> cUpdates;
        for (auto &c: scene.getPool<NodeAnimationComponent>()) {
            if (scene.checkComponent<SkinnedModelComponent>(c.entity)) {
            }
        }

        for (auto &pair: cUpdates) {
            scene.updateComponent(pair.first, pair.second);
        }
    }

    void NodeAnimationSystem::onComponentCreate(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == SkinnedModelComponent::typeName
            || component.getTypeName() == NodeAnimationComponent::typeName) {
        }
    }

    void NodeAnimationSystem::onComponentDestroy(const EntityHandle &entity, const Component &component) {
        if (component.getTypeName() == SkinnedModelComponent::typeName
            || component.getTypeName() == NodeAnimationComponent::typeName) {
        }
    }

    void NodeAnimationSystem::onComponentUpdate(const EntityHandle &entity,
                                               const Component &oldComponent,
                                               const Component &newComponent) {
        if (oldComponent.getTypeName() == SkinnedModelComponent::typeName) {
        } else if (oldComponent.getTypeName() == NodeAnimationComponent::typeName) {
            auto &oc = down_cast<const NodeAnimationComponent &>(oldComponent);
            auto &nc = down_cast<const NodeAnimationComponent &>(newComponent);
            for (auto &pair: oc.channels) {
                if (nc.channels.find(pair.first) == nc.channels.end()) {
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

                }
            }
        }
    }

    void NodeAnimationSystem::onEntityDestroy(const EntityHandle &entity) {
    }
}
