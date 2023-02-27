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

#ifndef XENGINE_RENDERCOMMAND_HPP
#define XENGINE_RENDERCOMMAND_HPP

#include "xng/gpu/vertexarraybuffer.hpp"

#include "xng/gpu/indexbuffer.hpp"

namespace xng {
    struct RenderCommand {
        size_t offset = 0; // The offset into the index or vertex buffer at which to begin reading indices or vertices
        size_t count = 0; // The number of indices or vertices to draw.
        IndexBuffer::IndexType indexType = IndexBuffer::UNSIGNED_INT; // The type of the indices, ignored when not indexing
    };
}

#endif //XENGINE_RENDERCOMMAND_HPP
