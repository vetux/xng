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

#ifndef XENGINE_MONOCPPDOMAIN_HPP
#define XENGINE_MONOCPPDOMAIN_HPP

#include <memory>

#include "script/mono/monocppassembly.hpp"

namespace xng {
    class XENGINE_EXPORT MonoCppDomain {
    public:
        MonoCppDomain();

        explicit MonoCppDomain(const std::string &domainName);

        ~MonoCppDomain();

        MonoCppAssembly &getMsCorLibAssembly();

        /**
         * Load assembly from file.
         *
         * @param filePath
         * @return
         */
        std::unique_ptr<MonoCppAssembly> loadAssembly(const std::string &filePath);

        /**
         * Load assembly from stream.
         *
         * @param source The stream pointing to the assembly data.
         * @return
         */
        std::unique_ptr<MonoCppAssembly> loadAssembly(std::istream &source);

        MonoCppObject stringFromUtf8(const std::string &str, bool pinned = false);

        std::string stringToUtf8(const MonoCppObject &strObject);

    private:
        MonoCppAssembly msCorLib;
        void *domainPointer;
    };
}

#endif //XENGINE_MONOCPPDOMAIN_HPP
