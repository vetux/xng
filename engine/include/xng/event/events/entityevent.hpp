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

#ifndef XENGINE_ENTITYEVENT_HPP
#define XENGINE_ENTITYEVENT_HPP

#include <typeindex>
#include <any>

#include "xng/event/event.hpp"
#include "xng/ecs/entityhandle.hpp"

namespace xng {
    struct EntityEvent : public Event {
        std::type_index getEventType() const override {
            return typeid(EntityEvent);
        }

        enum Type {
            ENTITY_ADD,
            ENTITY_REMOVE,
            COMPONENT_ADD,
            COMPONENT_REMOVE,
            COMPONENT_UPDATE
        } type;

        EntityHandle entity;
        std::type_index componentType;
        std::any component;
        std::any secondComponent;
    };
}

#endif //XENGINE_ENTITYEVENT_HPP
