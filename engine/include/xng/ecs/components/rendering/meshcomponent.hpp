/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_MESHCOMPONENT_HPP
#define XENGINE_MESHCOMPONENT_HPP

#include "xng/graphics/scene/material.hpp"
#include "xng/graphics/scene/mesh.hpp"
#include "xng/resource/resourcehandle.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    struct XENGINE_EXPORT MeshComponent final : Component {
        XNG_COMPONENT_TYPENAME(MeshComponent)

        bool castShadows{};
        bool receiveShadows{};

        ResourceHandle<Mesh> mesh;

        bool operator==(const MeshComponent &other) const {
            return enabled == other.enabled
                   && castShadows == other.castShadows
                   && receiveShadows == other.receiveShadows
                   && mesh == other.mesh;
        }

        Messageable &operator<<(const Message &message) override {
            message.value("castShadows", castShadows);
            message.value("receiveShadows", receiveShadows);
            message.value("mesh", mesh);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            castShadows >> message["castShadows"];
            receiveShadows >> message["receiveShadows"];
            mesh >> message["mesh"];
            return message;
        }
    };
}

#endif //XENGINE_MESHCOMPONENT_HPP
