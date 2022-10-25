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

#ifndef XENGINE_DRIVERREGISTRY_HPP
#define XENGINE_DRIVERREGISTRY_HPP

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <stdexcept>

#include "driver/driver.hpp"

namespace xng {
    class XENGINE_EXPORT DriverRegistry {
    public:
        typedef std::function<Driver *()> DriverCreator;

        /**
         * Retrieve the available driver creators for a given driver base class.
         *
         * @tparam T The driver base class
         * @return
         */
        template<typename T>
        static const std::map<std::string, DriverCreator> &getAvailableDrivers() {
            return drivers[typeid(T)];
        }

        /**
         * @tparam T The driver base class.
         * @param name The unique identifier for the driver base class implementation
         * @return A new instance of the driver implementation
         */
        template<typename T>
        static std::unique_ptr<T> load(const std::string &name) {
            auto dIt = drivers.find(typeid(T));
            if (dIt == drivers.end()) {
                throw std::runtime_error("No driver found for type " + std::string(typeid(T).name()) + " with name " + name);
            }
            auto nIt = dIt->second.find(name);
            if (nIt == dIt->second.end()){
                throw std::runtime_error("No driver found for type " + std::string(typeid(T).name()) + " with name " + name);
            }
            return std::unique_ptr<T>(dynamic_cast<T *>(drivers.at(typeid(T)).at(name)()));
        }

        /**
         * Register a driver creator for a given name.
         *
         * @param name The name under which the driver will be accessible in load()
         * @param baseType The base type of the driver, baseType must inherit xng::Driver, and the type of the object returned by the creator must inherit baseType.
         * @param creator The function object which constructors new instances of the base type implementation.
         * @return True if the driver was registered, or false if there was already a driver of the given base type with the given name.
         */
        static bool registerDriver(const std::string &name, std::type_index baseType, DriverCreator creator) noexcept;

    private:
        static std::map<std::type_index, std::map<std::string, DriverCreator>> drivers; // The driver creators for each base class
    };
}

#endif //XENGINE_DRIVERREGISTRY_HPP
