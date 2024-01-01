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

#ifndef XENGINE_GENERICCOMPONENT_HPP
#define XENGINE_GENERICCOMPONENT_HPP

#include "xng/ecs/component.hpp"

namespace xng {
    /**
     * When serializing a component for every entry in components is created.
     * When unknown type names are encountered when deserializing the data is stored in a generic component.
     */
    struct GenericComponent : public Component {
        std::map<std::string, Message> components;

        std::type_index getType() const override {
            return typeid(GenericComponent);
        }
    };
}

#endif //XENGINE_GENERICCOMPONENT_HPP
