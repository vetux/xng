/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_SURFACE_HPP
#define XENGINE_SURFACE_HPP

#include "xng/math/vector2.hpp"

#include "xng/rendergraph/textureproperties.hpp"

namespace xng::rendergraph {
    /**
     * A surface represents a display environment render target into which passes can render.
     * Any given Runtime.execute() call may render to multiple surfaces.
     *
     * The runtime manages swap chains for each surface.
     */
    class Surface {
    public:
        virtual ~Surface() = default;

        virtual Vec2i getDimensions() = 0;

        virtual ColorFormat getFormat() = 0;
    };
}

#endif //XENGINE_SURFACE_HPP