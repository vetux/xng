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

#ifndef XENGINE_RECTTRANSFORMCOMPONENT_HPP
#define XENGINE_RECTTRANSFORMCOMPONENT_HPP

#include "io/messageable.hpp"

namespace xng {
    /**
     * If the entity contains a TransformComponent the position(.x, .y) and rotation(.z) are added to the rect transform values.
     */
    struct XENGINE_EXPORT RectTransformComponent : public Messageable {
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

        bool enabled;

        Rectf rect;
        Vec2f center;
        float rotation;

        std::string parent;

        Messageable &operator<<(const Message &message) override {
            anchor = convertAnchor(message.value("anchor", std::string("top_left")));
            enabled = message.value("enabled", true);
            rect << message.value("rect");
            center << message.value("center");
            rotation = message.value("rotation", 0.0f);
            parent = message.value("parent", std::string());
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            message["anchor"] = convertAnchor(anchor);
            message["enabled"] = enabled;
            rect >> message["rect"];
            center >> message["center"];
            message["rotation"] = rotation;
            message["parent"] = parent;
            return message;
        }

        static Anchor convertAnchor(const std::string &str) {
            if (str == "top_left") {
                return TOP_LEFT;
            } else if (str == "top_center") {
                return TOP_CENTER;
            } else if (str == "top_right") {
                return TOP_RIGHT;
            } else if (str == "left") {
                return LEFT;
            } else if (str == "center") {
                return CENTER;
            } else if (str == "right") {
                return RIGHT;
            } else if (str == "bottom_left") {
                return BOTTOM_LEFT;
            } else if (str == "bottom_center") {
                return BOTTOM_CENTER;
            } else if (str == "bottom_right") {
                return BOTTOM_RIGHT;
            } else {
                throw std::runtime_error("Invalid anchor value");
            }
        }

        static std::string convertAnchor(Anchor anchor) {
            switch (anchor) {
                case TOP_LEFT:
                    return "top_left";
                case TOP_CENTER:
                    return "top_center";
                case TOP_RIGHT:
                    return "top_right";
                case LEFT:
                    return "left";
                case CENTER:
                    return "center";
                case RIGHT:
                    return "right";
                case BOTTOM_LEFT:
                    return "bottom_left";
                case BOTTOM_CENTER:
                    return "bottom_center";
                case BOTTOM_RIGHT:
                    return "bottom_right";
                default:
                    throw std::runtime_error("Invalid anchor");
            }
        }

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
#endif //XENGINE_RECTTRANSFORMCOMPONENT_HPP
