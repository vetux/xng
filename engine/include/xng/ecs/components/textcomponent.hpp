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

#ifndef XENGINE_TEXTCOMPONENT_HPP
#define XENGINE_TEXTCOMPONENT_HPP

#include "xng/io/messageable.hpp"
#include "xng/asset/rawasset.hpp"
#include "xng/text/alignment.hpp"
#include "canvastransformcomponent.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * A text component renders the given text.
     * If the resulting rendered text size is larger than the rect dimensions the text is cropped
     */
    struct XENGINE_EXPORT TextComponent : public Component {
        std::string text;

        Vec2i pixelSize;

        int lineHeight;
        int lineWidth;
        int lineSpacing;
        Alignment alignment;

        ResourceHandle<RawAsset> font;

        CanvasTransformComponent::Anchor textAnchor; // Controls where the rendered text is laid out in the rect

        Vec2f textScroll;

        ColorRGBA textColor;
        int layer; // The render layer of the text

        static std::string convertAlignment(Alignment alignment) {
            switch (alignment) {
                default:
                case ALIGN_LEFT:
                    return "left";
                case ALIGN_CENTER:
                    return "center";
                case ALIGN_RIGHT:
                    return "right";
            }
        }

        static Alignment convertAlignment(const std::string &str) {
            if (str == "left")
                return ALIGN_LEFT;
            else if (str == "center")
                return ALIGN_CENTER;
            else
                return ALIGN_RIGHT;
        }

        Messageable &operator<<(const Message &message) override {
            pixelSize << message.value("pixelSize");
            lineHeight = message.value("lineHeight", 0);
            lineWidth = message.value("lineWidth", 0);
            lineSpacing = message.value("lineSpacing", 0);
            font << message.value("font");
            alignment = convertAlignment(message.value("alignment", convertAlignment(ALIGN_LEFT)));
            textAnchor = CanvasTransformComponent::convertAnchor(message.value("textAnchor",
                                                                               CanvasTransformComponent::convertAnchor(
                                                                                       CanvasTransformComponent::LEFT)));
            textScroll << message.value("textScroll");
            text = message.value("text", std::string());
            textColor << message.value("textColor");
            layer = message.value("layer", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            pixelSize >> message["pixelSize"];
            message["lineHeight"] = lineHeight;
            message["lineWidth"] = lineWidth;
            message["lineSpacing"] = lineSpacing;
            font >> message["font"];
            message["alignment"] = convertAlignment(alignment);
            message["textAnchor"] = CanvasTransformComponent::convertAnchor(textAnchor);
            message["text"] = text;
            textScroll >> message["textScroll"];
            textColor >> message["textColor"];
            message["layer"] = layer;
            return message;
        }

        std::type_index getType() const override {
            return typeid(TextComponent);
        }
    };
}

#endif //XENGINE_TEXTCOMPONENT_HPP
