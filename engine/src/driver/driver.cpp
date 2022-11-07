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

#include "xng/driver/driver.hpp"

#include "compiled_drivers.h"

namespace xng {
    static bool _init = false;
    static std::map<std::string, Driver::Creator> _drivers;

    const std::map<std::string, Driver::Creator> &getDrivers() {
        if (!_init){
            _init = true;
            DriverGenerator::setup(_drivers);
        }
        return _drivers;
    }

    std::unique_ptr<Driver> Driver::load(const std::string &name) {
        auto &drivers = getDrivers();
        auto it = drivers.find(name);
        if (it == drivers.end()){
            throw std::runtime_error("No driver with name " + name + " found.");
        }
        return it->second();
    }
}