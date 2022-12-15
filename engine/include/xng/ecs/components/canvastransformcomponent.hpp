/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#ifndef XENGINE_CANVASTRANSFORMCOMPONENT_HPP
#define XENGINE_CANVASTRANSFORMCOMPONENT_HPP

#include "xng/io/messageable.hpp"

#include "xng/ecs/entity.hpp"
#include "canvascomponent.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * If the entity contains a TransformComponent the position(.x, .y) and rotation(.z) are added to the rect transform values.
     */
    struct XENGINE_EXPORT CanvasTransformComponent : public Component {
        enum Anchor {
            TOP_LEFT,
            TOP_CENTER,
            TOP_RIGHT,
            LEFT,
            CENTER,
            RIGHT,
            BOTTOM_LEFT,
            BOTTOM_CENTER,
            BOTTOM_RIGHT
        } anchor = TOP_LEFT;

        std::string canvas;

        Rectf rect;
        Vec2f center;
        float rotation;

        Messageable &operator<<(const Message &message) override {
            anchor = (Anchor) message.getMessage("anchor", Message((int)TOP_LEFT)).asInt();
            rect << message.getMessage("rect");
            center << message.getMessage("center");
            message.value("rotation", rotation);
            message.value("canvas", canvas);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["anchor"] = (int)anchor;
            message["enabled"] = enabled;
            rect >> message["rect"];
            center >> message["center"];
            message["rotation"] = rotation;
            message["canvas"] = canvas;
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(CanvasTransformComponent);
        }

        Vec2f getOffset(EntityScene &scene, Vec2i screenSize) const {
            auto &c = scene.getEntity(canvas).getComponent<CanvasComponent>();
            return getOffset(anchor, c.projectionSize.magnitude() > 0
                                     ? c.projectionSize
                                     : screenSize.convert<float>());
        }

    private:
        static Vec2f getOffset(Anchor anchor, const Vec2f &canvasSize) {
            switch (anchor) {
                default:
                case TOP_LEFT:
                    return {};
                case TOP_CENTER:
                    return {canvasSize.x / 2, 0};
                case TOP_RIGHT:
                    return {canvasSize.x, 0};
                case LEFT:
                    return {0, canvasSize.y / 2};
                case CENTER:
                    return {canvasSize.x / 2, canvasSize.y / 2};
                case RIGHT:
                    return {canvasSize.x, canvasSize.y / 2};
                case BOTTOM_LEFT:
                    return {0, canvasSize.y};
                case BOTTOM_CENTER:
                    return {canvasSize.x / 2, canvasSize.y};
                case BOTTOM_RIGHT:
                    return {canvasSize.x, canvasSize.y};
            }
        }
    };
}
#endif //XENGINE_CANVASTRANSFORMCOMPONENT_HPP
