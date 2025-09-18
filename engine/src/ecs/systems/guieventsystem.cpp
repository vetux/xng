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
#include "xng/ecs/components/spritecomponent.hpp"
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
         /*   auto &rt = scene.getComponent<RectTransformComponent>(pair.first);
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
                    if (clickButtons.find(pair.first) == clickButtons.end()) {
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

            if (sprite.assigned() && scene.checkComponent<SpriteComponent>(pair.first)) {
                auto comp = scene.getComponent<SpriteComponent>(pair.first);
                comp.sprite = sprite;
                scene.updateComponent(pair.first, comp);
            }*/
        }
    }
}