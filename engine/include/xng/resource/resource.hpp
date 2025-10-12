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

#ifndef XENGINE_RESOURCE_HPP
#define XENGINE_RESOURCE_HPP

#include <memory>

#define RESOURCE_TYPENAME(T) static constexpr auto typeName = #T; std::string getTypeName() const override { return typeName; }

namespace xng {
    class XENGINE_EXPORT Resource {
    public:
        Resource() {
        }

        virtual ~Resource() = default;

        virtual std::unique_ptr<Resource> clone() = 0;

        virtual std::string getTypeName() const = 0;

        /**
         * @return Wheter or not the dependencies of this resource are loaded.
         */
        virtual bool isLoaded() const { return true; }
    };
}

#endif //XENGINE_RESOURCE_HPP
