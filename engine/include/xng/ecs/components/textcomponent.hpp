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
#include "xng/resource/rawresource.hpp"
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

        ResourceHandle<RawResource> font;

        CanvasTransformComponent::Anchor textAnchor; // Controls where the rendered text is laid out in the rect

        Vec2f textScroll;

        ColorRGBA textColor;
        int layer; // The render layer of the text

        Messageable &operator<<(const Message &message) override {
            pixelSize << message.getMessage("pixelSize");
            message.value("lineHeight", lineHeight);
            message.value("lineWidth", lineWidth);
            message.value("lineSpacing", lineSpacing);
            font << message.getMessage("font");
            alignment = (Alignment) message.getMessage("alignment", Message((int) ALIGN_LEFT)).asInt();
            textAnchor = (CanvasTransformComponent::Anchor) message.getMessage("textAnchor",
                                                                          Message((int) CanvasTransformComponent::LEFT)).asInt();
            textScroll << message.getMessage("textScroll");
            message.value("text", text);
            textColor << message.getMessage("textColor");
            message.value("layer", layer);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            pixelSize >> message["pixelSize"];
            message["lineHeight"] = lineHeight;
            message["lineWidth"] = lineWidth;
            message["lineSpacing"] = lineSpacing;
            font >> message["font"];
            message["alignment"] = (int)alignment;
            message["textAnchor"] = (int)textAnchor;
            message["text"] = text;
            textScroll >> message["textScroll"];
            textColor >> message["textColor"];
            message["layer"] = layer;
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(TextComponent);
        }
    };
}

#endif //XENGINE_TEXTCOMPONENT_HPP
