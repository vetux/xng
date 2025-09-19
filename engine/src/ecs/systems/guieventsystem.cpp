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

#include "xng/ecs/systems/guieventsystem.hpp"

#include "xng/ecs/components/gui/buttoncomponent.hpp"
#include "xng/ecs/components/rendering/spritecomponent.hpp"
#include "xng/event/events/guievent.hpp"
#include "xng/input/device/mouse.hpp"
#include "xng/util/time.hpp"

// TODO: Reimplement gui event system
namespace xng {
    GuiEventSystem::GuiEventSystem(Window &window)
            : window(window) {}

    void GuiEventSystem::start(EntityScene &scene, EventBus &eventBus) {}

    void GuiEventSystem::stop(EntityScene &scene, EventBus &eventBus) {
        hoverButtons.clear();
    }

    void GuiEventSystem::update(DeltaTime deltaTime, EntityScene &scene, EventBus &eventBus) {
        for (auto &pair: scene.getPool<ButtonComponent>()) {
         /*   auto &rt = scene.getComponent<RectTransformComponent>(pair.entity);
            auto windowSize = window.getRenderTarget().getDescription().size;
           auto canvas = scene.getEntity(rt.canvas).getComponent<CanvasComponent>();
            ResourceHandle<Sprite> sprite;
            auto scale = canvas.getViewportScale(windowSize);
            auto rect = Rectf((rt.rect.position + rt.getOffset(scene, windowSize)
                               + Vec2f(-rt.center.x, rt.center.y)) * scale,
                              rt.rect.dimensions * scale);
            auto mousePos = (window.getInput().getDevice<Mouse>().position.convert<float>())
                            - canvas.getViewportOffset(windowSize).convert<float>();
            if (rect.testPoint(mousePos)) {
                if (window.getInput().getDevice<Mouse>().getButton(LEFT)) {
                    if (clickButtons.find(pair.entity) == clickButtons.end()) {
                        // Pressing
                        eventBus.invoke(GuiEvent(GuiEvent::BUTTON_CLICK, pair.component.id));
                        clickButtons.insert(pair.entity);
                    }
                    sprite = pair.component.spritePressed;
                } else {
                    clickButtons.erase(pair.entity);
                    // Hovering
                    if (hoverButtons.find(pair.entity) == hoverButtons.end()) {
                        eventBus.invoke(GuiEvent(GuiEvent::BUTTON_HOVER_START, pair.component.id));
                    }
                    sprite = pair.component.spriteHover;
                }
                hoverButtons.insert(pair.entity);
            } else {
                // Not hovering
                clickButtons.erase(pair.entity);
                if (hoverButtons.find(pair.entity) != hoverButtons.end()) {
                    eventBus.invoke(GuiEvent(GuiEvent::BUTTON_HOVER_STOP, pair.component.id));
                    hoverButtons.erase(pair.entity);
                }
                sprite = pair.component.sprite;
            }

            if (sprite.assigned() && scene.checkComponent<SpriteComponent>(pair.entity)) {
                auto comp = scene.getComponent<SpriteComponent>(pair.entity);
                comp.sprite = sprite;
                scene.updateComponent(pair.entity, comp);
            }*/
        }
    }
}