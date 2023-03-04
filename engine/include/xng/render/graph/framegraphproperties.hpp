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

#ifndef XENGINE_FRAMEGRAPHPROPERTIES_HPP
#define XENGINE_FRAMEGRAPHPROPERTIES_HPP

#include <string>

#define USER_PROPERTY(name) static const char *name = #name;

namespace xng {
    /**
     * The keys of the optional default properties supplied by the renderer.
     * Frame graphs can define other properties.
     */
    namespace FrameGraphProperties {
        // float, Range(0, inf) Render resolution is backbuffer size * scale, if not defined the full backbuffer resolution is used, the rendered images are projected to fit the backbuffer resolution.
        USER_PROPERTY(RENDER_SCALE)

        // int, the number of sub samples per pixel (MSAA) to use when forward rendering, if not defined the back buffer sample count is used.
        USER_PROPERTY(RENDER_SAMPLES)
    }
}

#endif //XENGINE_FRAMEGRAPHPROPERTIES_HPP
