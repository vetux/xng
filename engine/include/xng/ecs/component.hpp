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

#ifndef XENGINE_COMPONENT_HPP
#define XENGINE_COMPONENT_HPP

#include "xng/io/messageable.hpp"

#define XNG_COMPONENT_TYPENAME(T) static constexpr auto typeName = "xng::"#T; std::string getTypeName() const override { return typeName; }

#define COMPONENT_TYPENAME(T) static constexpr auto typeName = #T;

namespace xng {
    /**
     * Component types must extend this type.
     */
    struct XENGINE_EXPORT Component : Messageable {
        ~Component() override = default;

        virtual std::string getTypeName() const = 0;

        Messageable &operator<<(const Message &message) override {
            message.value("enabled", enabled, true);
            return *this;
        }

        /**
         * @param message Must be of type DICTIONARY
         * @return
         */
        Message &operator>>(Message &message) const override {
            enabled >> message["enabled"];
            return message;
        }

        bool operator==(const Component &other) const {
            return enabled == other.enabled;
        }

        bool enabled = true;
    };
}

#endif //XENGINE_COMPONENT_HPP
