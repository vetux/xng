/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_AUDIOSOURCECOMPONENT_HPP
#define XENGINE_AUDIOSOURCECOMPONENT_HPP

#include "xng/ecs/component.hpp"
#include "xng/audio/audiodata.hpp"
#include "xng/math/vector3.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT AudioSourceComponent : public Component {
        ResourceHandle<AudioData> audio;
        bool play = false;
        bool loop = false;
        Vec3f velocity = {};

        bool operator==(const AudioSourceComponent &other) const {
            return audio == other.audio
                   && play == other.play
                   && loop == other.loop
                   && velocity == other.velocity;
        }

        bool operator!=(const AudioSourceComponent &other) const {
            return !(*this == other);
        }

        Messageable &operator<<(const Message &message) override {
            message.value("audio", audio);
            message.value("play", play);
            message.value("loop", loop);
            message.value("velocity", velocity);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            audio >> message["audio"];
            play >> message["play"];
            loop >> message["loop"];
            velocity >> message["velocity"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(AudioSourceComponent);
        }
    };
}

#endif //XENGINE_AUDIOSOURCECOMPONENT_HPP
