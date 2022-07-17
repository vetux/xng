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

#ifndef XENGINE_DRIVERREGISTRY_HPP
#define XENGINE_DRIVERREGISTRY_HPP

#include <string>
#include <map>
#include <memory>
#include <functional>

#include "driver/driver.hpp"

namespace xng {
    class XENGINE_EXPORT DriverRegistry {
    public:
        typedef std::function<Driver *()> DriverCreator;

        static const std::map<std::string, DriverCreator> &getAvailableDrivers();

        static Driver *loadDriver(const std::string &name);

        template<typename T>
        static std::unique_ptr<T> load(const std::string &name) {
            return std::unique_ptr<T>(dynamic_cast<T *>(loadDriver(name)));
        }

        static bool registerDriver(const std::string &name, DriverCreator creator) noexcept;
    };
}

#endif //XENGINE_DRIVERREGISTRY_HPP
