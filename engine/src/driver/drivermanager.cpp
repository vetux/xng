/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "driver/drivermanager.hpp"

#include <stdexcept>
#include <utility>

namespace xengine {
    static std::map<std::string, std::function<Driver *()>> drivers;

    const std::map<std::string, std::function<Driver *()>> &DriverManager::getAvailableDrivers() {
        return drivers;
    }

    Driver *DriverManager::loadDriver(const std::string &name) {
        return drivers.at(name)();
    }

    bool DriverManager::registerDriver(const std::string &name, DriverManager::DriverCreator creator) noexcept {
        if (drivers.find(name) != drivers.end()) {
            return false;
        } else {
            drivers[name] = std::move(creator);
            return true;
        }
    }
}