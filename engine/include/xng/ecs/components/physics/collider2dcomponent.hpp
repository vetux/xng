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

#ifndef XENGINE_COLLIDER2DCOMPONENT_HPP
#define XENGINE_COLLIDER2DCOMPONENT_HPP

#include "xng/physics/rigidbody.hpp"
#include "xng/io/messageable.hpp"
#include "xng/ecs/component.hpp"

namespace xng{
    /**
     * 2D collider, Collider shapes must be COLLIDER_2D
     */
    struct XENGINE_EXPORT Collider2DComponent : public Component {
        std::vector<ColliderDesc> colliders;

        Messageable &operator<<(const Message &message) override {
            message.value("colliders", colliders);
            return Component::operator<<(message);
        }

        Message &operator>>(Message &message) const override {
            colliders >> message["colliders"];
            return Component::operator>>(message);
        }

        std::type_index getType() const override {
            return typeid(Collider2DComponent);
        }
    };
}

#endif //XENGINE_COLLIDER2DCOMPONENT_HPP
