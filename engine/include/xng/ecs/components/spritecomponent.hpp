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

#ifndef XENGINE_SPRITECOMPONENT_HPP
#define XENGINE_SPRITECOMPONENT_HPP

#include "xng/asset/sprite.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct SpriteComponent : public Component {
        ResourceHandle<Sprite> sprite; // The sprite to draw
        ResourceHandle<Sprite> spriteB; // If assigned the resulting sprite is the blend between sprite and spriteB colors.
        float blendScale = 0;
        int layer = 0; // The render layer of the sprite on this canvas
        Vec2b flipSprite;
        float mix = 0;
        ColorRGB mixColor = {};

        Messageable &operator<<(const Message &message) override {
            sprite << message.value("sprite");
            spriteB << message.value("spriteB");
            blendScale = message.value("blendScale", 0.0f);
            layer = message.value("layer", 0);
            flipSprite << message.value("flipSprite");
            mix = message.value("mix", 0.0f);
            mixColor << message.value("mixColor");
            Component::operator<<(message);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            sprite >> message["sprite"];
            spriteB >> message["spriteB"];
            message["blendScale"] = blendScale;
            message["layer"] = layer;
            flipSprite >> message["flipSprite"];
            message["mix"] = mix;
            mixColor >> message["mixColor"];
            Component::operator>>(message);
            return message;
        }

        std::type_index getType() const override {
            return typeid(SpriteComponent);
        }
    };
}

#endif //XENGINE_SPRITECOMPONENT_HPP
