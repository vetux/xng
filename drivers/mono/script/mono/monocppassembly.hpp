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

#ifndef XENGINE_MONOCPPASSEMBLY_HPP
#define XENGINE_MONOCPPASSEMBLY_HPP

#include <string>

#include "script/mono/monocppobject.hpp"
#include "script/mono/monocpparguments.hpp"
#include "script/mono/monocppvalue.hpp"

namespace xng {
    //TODO: Refactor mono abstraction
    class XENGINE_EXPORT MonoCppAssembly {
    public:
        MonoCppAssembly() = default;

        explicit MonoCppAssembly(void *domainPointer,
                                 void *assemblyPointer,
                                 void *imagePointer);

        ~MonoCppAssembly();

        MonoCppObject invokeStaticMethod(const std::string &nameSpace,
                                         const std::string &className,
                                         const std::string &functionName,
                                         const MonoCppArguments &args = {}) const;

        void setStaticField(const std::string &nameSpace,
                            const std::string &className,
                            const std::string &fieldName,
                            MonoCppValue value) const;

        MonoCppObject getStaticField(const std::string &nameSpace,
                                     const std::string &className,
                                     const std::string &fieldName) const;

        MonoCppObject createObject(const std::string &nameSpace,
                                   const std::string &className,
                                   bool pinned = false) const;

        void *domainPointer;
        void *assemblyPointer;
        void *imagePointer;

        std::string imageBytes;
    };
}

#endif //XENGINE_MONOCPPASSEMBLY_HPP
