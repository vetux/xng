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

#ifndef XENGINE_SCRIPTINGSYSTEM_H
#define XENGINE_SCRIPTINGSYSTEM_H

#include "ecs/system.hpp"

#include "script/mono/monoscript.hpp"
#include "script/mono/monocppassembly.hpp"

#include "input/input.hpp"
#include "io/archive.hpp"

namespace xengine {
    class XENGINE_EXPORT MonoScriptingSystem : public System, InputListener {
    public:
        MonoScriptingSystem(Input &input,
                            MonoCppDomain &domain,
                            MonoCppAssembly &manaAssembly,
                            Archive &archive);

        ~MonoScriptingSystem() override = default;

        void start(EntityManager &entityManager) override;

        void stop(EntityManager &entityManager) override;

        void update(float deltaTime, EntityManager &entityManager) override;

    private:
        Input &input;
        Archive &archive;
        MonoCppDomain &domain;
        MonoCppAssembly &msCorLib;
        MonoCppAssembly &manaAssembly;

        MonoCppAssembly &getAssembly(const std::string &path);

        std::map<std::string, std::unique_ptr<MonoCppAssembly>> assemblies;
    };
}

#endif //XENGINE_SCRIPTINGSYSTEM_H
