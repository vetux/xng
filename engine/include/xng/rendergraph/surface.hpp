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

#ifndef XENGINE_SURFACE_HPP
#define XENGINE_SURFACE_HPP

#include "xng/math/vector2.hpp"

#include "xng/rendergraph/textureproperties.hpp"

namespace xng::rg {
    /**
     * A surface represents a display environment render target into which passes can render.
     *
     * Any given Runtime.execute() call may render to multiple surfaces.
     *
     * The runtime manages swap chains for each surface.
     */
    class Surface {
    public:
        virtual ~Surface() = default;

        /**
         * The runtime internally caches the dimensions of the frame buffer.
         *
         * At the end of each Runtime.execute() invocation, the runtime checks the current window frame buffer size
         * and reconfigures the swap chain when needed.
         *
         * This means the returned value may be unstable across graph executions.
         *
         * @return The current dimensions of the surface (Swap chain images).
         */
        virtual Vec2i getDimensions() = 0;
    };
}

#endif //XENGINE_SURFACE_HPP