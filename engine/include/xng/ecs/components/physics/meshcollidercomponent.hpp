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

#ifndef XENGINE_MESHCOLLIDERCOMPONENT_HPP
#define XENGINE_MESHCOLLIDERCOMPONENT_HPP

#include "xng/physics/rigidbody.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * A mesh collider for the physics system.
     */
    struct XENGINE_EXPORT MeshColliderComponent final : Component {
        XNG_COMPONENT_TYPENAME(MeshColliderComponent)

        ResourceHandle<Mesh> mesh;
        ColliderShapeType shapeType{}; // Either COLLIDER_CONVEX_HULL or COLLIDER_TRIANGLES
        ColliderProperties properties{};

        Messageable &operator<<(const Message &message) override {
            message.value("mesh", mesh);
            message.value("shapeType", reinterpret_cast<int&>(shapeType));
            message.value("properties", properties);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            mesh >> message["mesh"];
            shapeType >> message["shapeType"];
            properties >> message["properties"];
            return Component::operator>>(message);
        }
    };
}

#endif //XENGINE_MESHCOLLIDERCOMPONENT_HPP