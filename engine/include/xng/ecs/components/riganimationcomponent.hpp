/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_RIGANIMATIONCOMPONENT_HPP
#define XENGINE_RIGANIMATIONCOMPONENT_HPP

#include "xng/ecs/component.hpp"
#include "xng/animation/skeletal/riganimation.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/util/time.hpp"

namespace xng {
    /**
     * Contains the animated bone transforms.
     */
    struct RigAnimationComponent final : Component {
        struct Channel final : Messageable {
            ResourceHandle<RigAnimation> animation;
            Duration blendDuration{};
            bool loop{};

            Messageable &operator<<(const Message &message) override {
                message.value("animation", animation);
                message.value("blendDuration", blendDuration);
                message.value("loop", loop);
                return *this;
            }

            Message &operator>>(Message &message) const override {
                message = Message(Message::DICTIONARY);
                animation >> message["animation"];
                blendDuration >> message["blendDuration"];
                loop >> message["loop"];
                return message;
            }
        };

        std::map<size_t, Channel> channels;
        std::map<std::string, Mat4f> boneTransforms;

        bool operator==(const RigAnimationComponent &other) const {
            return enabled == other.enabled
                   && channels == other.channels
                   && boneTransforms == other.boneTransforms;
        }

        Messageable &operator<<(const Message &message) override {
            message.value("channels", channels);
            message.value("boneTransforms", boneTransforms);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            channels >> message["channels"];
            boneTransforms >> message["boneTransforms"];
            return message;
        }

        std::type_index getType() const override {
            return typeid(RigAnimationComponent);
        }
    };
}

#endif //XENGINE_RIGANIMATIONCOMPONENT_HPP
