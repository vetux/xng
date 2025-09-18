/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_CONTACTEVENT_HPP
#define XENGINE_CONTACTEVENT_HPP

#include "xng/event/event.hpp"

#include "xng/physics/world.hpp"
#include "xng/ecs/entityhandle.hpp"

namespace xng {
    struct ContactEvent {
        EVENT_TYPENAME(ContactEvent)

        enum Type {
            BEGIN_CONTACT,
            END_CONTACT
        } type{};

        EntityHandle entityA;
        EntityHandle entityB;

        size_t colliderIndexA{};
        size_t colliderIndexB{};

        ContactEvent() = default;

        ContactEvent(const Type type,
                     const EntityHandle entityA,
                     const EntityHandle entityB,
                     const size_t colliderIndexA,
                     const size_t colliderIndexB)
            : type(type),
              entityA(entityA),
              entityB(entityB),
              colliderIndexA(colliderIndexA),
              colliderIndexB(colliderIndexB) {
        }
    };
}

#endif //XENGINE_CONTACTEVENT_HPP
