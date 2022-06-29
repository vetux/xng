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

#ifndef XENGINE_PROTOCOL_HPP
#define XENGINE_PROTOCOL_HPP

#include <ostream>

#include "message.hpp"

namespace xng {
    class XENGINE_EXPORT Protocol {
    public:
        virtual void serialize(std::ostream &stream, const Message &message) = 0;

        virtual Message deserialize(std::istream &stream) = 0;
    };
}

#endif //XENGINE_PROTOCOL_HPP
