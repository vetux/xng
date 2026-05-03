/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_RESOURCE_HPP
#define XENGINE_RESOURCE_HPP

#include <memory>
#include <string>

#define RESOURCE_TYPENAME(T) static constexpr auto typeName = #T; std::string getTypeName() const override { return typeName; }

namespace xng {
    class XENGINE_EXPORT ResourceBase {
    public:
        ResourceBase() = default;

        virtual ~ResourceBase() = default;

        virtual std::unique_ptr<ResourceBase> clone() = 0;

        virtual std::string getTypeName() const = 0;

        /**
         * @return Wheter or not the dependencies of this resource are loaded.
         */
        virtual bool isLoaded() const { return true; }
    };
}

#endif //XENGINE_RESOURCE_HPP
