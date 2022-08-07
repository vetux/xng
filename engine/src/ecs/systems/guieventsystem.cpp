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

#include "ecs/systems/guieventsystem.hpp"

#include "ecs/components/recttransformcomponent.hpp"
#include "ecs/components/buttoncomponent.hpp"
#include "ecs/components/spritecomponent.hpp"
#include "event/events/guievent.hpp"

namespace xng {
    GuiEventSystem::GuiEventSystem(Window &window, EventBus &eventBus)
            : window(window), eventBus(eventBus) {}

    void GuiEventSystem::start(EntityScene &scene) {}

    void GuiEventSystem::stop(EntityScene &scene) {
        hoverButtons.clear();
    }

    void GuiEventSystem::update(DeltaTime deltaTime, EntityScene &scene) {
        for (auto &pair: scene.getPool<ButtonComponent>()) {
            auto &rt = scene.lookup<RectTransformComponent>(pair.first);
            ResourceHandle<Sprite> sprite;
            auto rect = Rectf(rt.rect.position + RectTransformComponent::getOffset(rt.anchor,
                                                                                   window.getRenderTarget().getDescription().size.convert<float>())
                              + Vec2f(0, rt.center.y)
                              - Vec2f(rt.center.x, 0),
                              rt.rect.dimensions);
            if (rect.testPoint(window.getInput().getMouse().position.convert<float>())) {
                if (window.getInput().getMouse().getButton(LEFT)) {
                    if (clickButtons.find(pair.first) == clickButtons.end()){
                        // Pressing
                        eventBus.invoke(GuiEvent(GuiEvent::BUTTON_CLICK, pair.second.id));
                        clickButtons.insert(pair.first);
                    }
                    sprite = pair.second.spritePressed;
                } else {
                    clickButtons.erase(pair.first);
                    // Hovering
                    if (hoverButtons.find(pair.first) == hoverButtons.end()) {
                        eventBus.invoke(GuiEvent(GuiEvent::BUTTON_HOVER_START, pair.second.id));
                    }
                    sprite = pair.second.spriteHover;
                }
                hoverButtons.insert(pair.first);
            } else {
                // Not hovering
                clickButtons.erase(pair.first);
                if (hoverButtons.find(pair.first) != hoverButtons.end()) {
                    eventBus.invoke(GuiEvent(GuiEvent::BUTTON_HOVER_STOP, pair.second.id));
                    hoverButtons.erase(pair.first);
                }
                sprite = pair.second.sprite;
            }

            if (sprite.assigned() && scene.check<SpriteComponent>(pair.first)) {
                auto comp = scene.lookup<SpriteComponent>(pair.first);
                comp.sprite = sprite;
                scene.updateComponent(pair.first, comp);
            }
        }
    }
}