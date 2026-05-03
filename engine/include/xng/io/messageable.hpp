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

#ifndef XENGINE_SERIALIZABLE_HPP
#define XENGINE_SERIALIZABLE_HPP

#include "xng/io/message.hpp"

namespace xng {
    class XENGINE_EXPORT Messageable {
    public:
        virtual ~Messageable() = default;

        virtual Messageable &operator<<(const Message &message) = 0;

        virtual Message &operator>>(Message &message) const = 0;

        virtual bool operator==(const Messageable &other) const {
            return false;
        }
    };
}
#endif //XENGINE_SERIALIZABLE_HPP
