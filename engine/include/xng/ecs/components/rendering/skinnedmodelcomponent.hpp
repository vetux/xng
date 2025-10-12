/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_SKINNEDMESHCOMPONENT_HPP
#define XENGINE_SKINNEDMESHCOMPONENT_HPP

#include "xng/graphics/scene/skinnedmodel.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT SkinnedModelComponent final : Component {
        XNG_COMPONENT_TYPENAME(SkinnedModelComponent)

        bool castShadows{};
        bool receiveShadows{};

        ResourceHandle<SkinnedModel> model;

        bool operator==(const SkinnedModelComponent &other) const {
            return enabled == other.enabled
                   && castShadows == other.castShadows
                   && receiveShadows == other.receiveShadows
                   && model == other.model;
        }

        Messageable &operator<<(const Message &message) override {
            message.value("castShadows", castShadows);
            message.value("receiveShadows", receiveShadows);
            message.value("model", model);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            castShadows >> message["castShadows"];
            receiveShadows >> message["receiveShadows"];
            model >> message["model"];
            return message;
        }
    };
}

#endif //XENGINE_SKINNEDMESHCOMPONENT_HPP
