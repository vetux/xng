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

#ifndef XENGINE_SYSTEMREGISTRY_HPP
#define XENGINE_SYSTEMREGISTRY_HPP

#include <typeindex>
#include <functional>
#include <string>
#include <map>
#include <memory>

#include "ecs/system.hpp"

namespace xng {
    /**
     * The system registry is used by the editor to instantiate user defined systems based on their name
     * by loading a shared library containing the register directives for the user system types at runtime.
     */
    class XENGINE_EXPORT SystemRegistry {
    public:
        static SystemRegistry &instance();

        void registerSystem(const std::type_index &typeIndex,
                            const std::string &typeName,
                            const std::function<std::unique_ptr<System>()> &constructor);

        const std::type_index &getTypeFromName(const std::string &typeName);

        const std::string &getTypeNameFromIndex(const std::type_index &index);

        std::unique_ptr<System> create(const std::type_index &index);

    private:
        SystemRegistry() = default;

        static std::unique_ptr<SystemRegistry> inst;

        std::map<std::type_index, std::string> typeNameMapping;
        std::map<std::string, std::type_index> typeNameReverseMapping;

        std::map<std::type_index, std::function<std::unique_ptr<System>()>> constructors;
    };
}
#endif //XENGINE_SYSTEMREGISTRY_HPP
