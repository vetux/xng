/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "io/messageable.hpp"

namespace xng {
    /**
     * Component types must extend this type.
     */
    struct XENGINE_EXPORT Component : public Messageable {
        virtual std::type_index getType() const = 0;

        Messageable &operator<<(const Message &message) override {
            throw std::runtime_error("Message insert operator not implemented in component");
        }

        Message &operator>>(Message &message) const override {
            throw std::runtime_error("Message extract operator not implemented in component");
        }
    };
}

#endif //XENGINE_COMPONENT_HPP
