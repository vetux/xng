/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include <memory>

#include "xng/gpu/renderobject.hpp"
#include "xng/gpu/renderbuffertype.hpp"
#include "xng/gpu/gpufence.hpp"

namespace xng {
    /**
     * A RenderBuffer represents a buffer of some sort that depending on the implementation may or may not be hardware backed.
     */
    class XENGINE_EXPORT RenderBuffer : public RenderObject {
    public:
        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        virtual std::unique_ptr<GpuFence> copy(RenderBuffer &source) = 0;

        /**
         * Copy the data in source buffer to this buffer.
         *
         * @param source The concrete type of other must be compatible and have the same properties as this buffer.
         * @return
         */
        virtual std::unique_ptr<GpuFence> copy(RenderBuffer &source,
                                               size_t readOffset,
                                               size_t writeOffset,
                                               size_t count) = 0;

        /**
         * @return The size of the buffer in bytes
         */
        virtual size_t getSize() = 0;

        virtual RenderBufferType getBufferType() = 0;
    };
}

#endif //XENGINE_RENDERBUFFER_HPP
