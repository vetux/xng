/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

    void SystemRegistry::registerSystem(const std::string &typeName,
                                        const std::function<std::unique_ptr<System>()> &constructor) {
        if (constructors.find(typeName) != constructors.end())
            throw std::runtime_error("System " + typeName + " already registered");
        constructors[typeName] = constructor;
    }

    std::unique_ptr<System> SystemRegistry::create(const std::string &typeName) const {
        return constructors.at(typeName)();
    }
}
