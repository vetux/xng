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

#ifndef XENGINE_SKINNEDMESHCOMPONENT_HPP
#define XENGINE_SKINNEDMESHCOMPONENT_HPP

#include "xng/ecs/component.hpp"

#include "xng/resource/resourcehandle.hpp"

namespace xng {
    struct XENGINE_EXPORT SkinnedModelComponent final : Component {
        XNG_COMPONENT_TYPENAME(SkinnedModelComponent)

        bool castShadows{};
        bool receiveShadows{};

        //ResourceHandle<SkinnedModel> model;

        bool operator==(const SkinnedModelComponent &other) const {
            return enabled == other.enabled
                   && castShadows == other.castShadows
                   && receiveShadows == other.receiveShadows;
        }

        Messageable &operator<<(const Message &message) override {
            message.value("castShadows", castShadows);
            message.value("receiveShadows", receiveShadows);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            castShadows >> message["castShadows"];
            receiveShadows >> message["receiveShadows"];
            return message;
        }
    };
}

#endif //XENGINE_SKINNEDMESHCOMPONENT_HPP
