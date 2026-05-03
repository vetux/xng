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

#ifndef XENGINE_ENTITYNAME_HPP
#define XENGINE_ENTITYNAME_HPP

#include <string>

#include "xng/io/messageable.hpp"

namespace xng {
    /**
     * String wrapper type used for user component members.
     * The editor allows dragging and dropping named entities on EntityName XVARIABLE()'s.
     */
    struct EntityName : public Messageable {
        std::string value;

         Messageable &operator<<(const Message &message) override {
            value = message.asString();
            return *this;
         }

         Message &operator>>(Message &message) const {
             message = Message(value);
             return message;
         }
    };
}
#endif //XENGINE_ENTITYNAME_HPP
