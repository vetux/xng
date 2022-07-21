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

#ifndef XENGINE_SPRITEKEYFRAME_HPP
#define XENGINE_SPRITEKEYFRAME_HPP

#include "resource/resourcehandle.hpp"

#include "asset/sprite.hpp"

namespace xng {
    struct SpriteKeyframe : public Messageable {
        SpriteKeyframe() = default;

        explicit SpriteKeyframe(ResourceHandle<Sprite> sprite, int duration = 1)
                : sprite(std::move(sprite)), duration(duration) {}

        Messageable &operator<<(const Message &message) override {
            sprite << message.value("sprite");
            duration = message.value("duration", 0);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            sprite >> message["sprite"];
            message["duration"] = duration;
            return message;
        }

        ResourceHandle<Sprite> sprite; // The sprite to display
        int duration{}; // The duration in frames for which the keyframe should be displayed, if larger than 1 essentially the same as duplicating the keyframes in the sprite animation.
    };
}
#endif //XENGINE_SPRITEKEYFRAME_HPP
