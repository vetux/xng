/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_AUDIOSOURCECOMPONENT_HPP
#define XENGINE_AUDIOSOURCECOMPONENT_HPP

#include "xng/ecs/component.hpp"
#include "xng/assets/audiodata.hpp"
#include "xng/math/vector3.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/io/messageable.hpp"

namespace xng {
    struct XENGINE_EXPORT AudioSourceComponent final : Component {
        XNG_COMPONENT_TYPENAME(AudioSourceComponent)

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
    };
}

#endif //XENGINE_AUDIOSOURCECOMPONENT_HPP
