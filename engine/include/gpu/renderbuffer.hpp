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

#ifndef XENGINE_RENDERBUFFER_HPP
#define XENGINE_RENDERBUFFER_HPP

#include "gpu/renderobject.hpp"
#include "gpu/renderbuffertype.hpp"

namespace xng {
    class RenderBuffer : public RenderObject {
    public:
        /**
         * Copy the data in other buffer to this buffer.
         *
         * @param other The concrete type of other must be compatible and have the same properties as this buffer.
         */
        virtual void copy(RenderBuffer &other) = 0;

        virtual RenderBufferType getBufferType() = 0;
    };
}

#endif //XENGINE_RENDERBUFFER_HPP
