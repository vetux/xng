/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_DRIVER_HPP
#define XENGINE_DRIVER_HPP

#include <typeindex>

namespace xng {
    /**
     * Classes which inherit from this interface act as an entry point to some interface.
     *
     * Implementations of these classes can be registered and retrieved at runtime through the driver registry.
     */
    class XENGINE_EXPORT Driver {
    public:
        virtual ~Driver() = default;

        /**
         * @return The base type of the driver (Accessible in engine/include, or User defined driver base type)
         */
        virtual std::type_index getBaseType() = 0;

        /**
         * @return The concrete type of the driver (Encapsulated in drivers/ and not accessible to the user, or User defined driver type)
         */
        virtual std::type_index getType() = 0;
    };
}

#endif //XENGINE_DRIVER_HPP
