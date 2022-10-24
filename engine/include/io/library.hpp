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

#ifndef XENGINE_LIBRARY_HPP
#define XENGINE_LIBRARY_HPP

#include <memory>
#include <string>

namespace xng {
    class XENGINE_EXPORT Library {
    public:
        /**
         * Load a shared library by path.
         *
         * @param path
         * @return
         */
        static std::unique_ptr<Library> load(const std::string &path);

        /**
         * Unload the library, returned symbol addresses become invalid.
         */
        virtual ~Library() = default;

        /**
         * Return the address for the given symbol name.
         *
         * @param name
         * @return
         */
        virtual void *getSymbolAddress(const std::string &name) = 0;

        /**
         * Return the symbol casted to a reference,
         * Not typesafe
         *
         * @tparam T
         * @param name
         * @return
         */
        template<typename T>
        T &getSymbol(const std::string &name) {
            return *((T *) getSymbolAddress(name));
        }
    };
}

#endif //XENGINE_LIBRARY_HPP
