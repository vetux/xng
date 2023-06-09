/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/ecs/systemregistry.hpp"

namespace xng {
    std::unique_ptr<SystemRegistry> SystemRegistry::inst = nullptr;

    SystemRegistry &SystemRegistry::instance() {
        if (inst == nullptr)
            inst = std::make_unique<SystemRegistry>();
        return *inst;
    }

    bool SystemRegistry::registerSystem(const std::type_index &typeIndex,
                                        const std::string &typeName,
                                        const std::function<std::unique_ptr<System>()> &constructor) noexcept {
        if (typeNameMapping.find(typeIndex) != typeNameMapping.end())
            return false;
        typeNameMapping[typeIndex] = typeName;
        typeNameReverseMapping.insert(std::pair(typeName, typeIndex));
        constructors[typeIndex] = constructor;
        return true;
    }

    const std::type_index &SystemRegistry::getTypeFromName(const std::string &typeName) {
        return typeNameReverseMapping.at(typeName);
    }

    const std::string &SystemRegistry::getTypeNameFromIndex(const std::type_index &index) {
        return typeNameMapping.at(index);
    }

    std::unique_ptr<System> SystemRegistry::create(const std::type_index &index) {
        return constructors.at(index)();
    }
}