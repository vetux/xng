/**
 *  xEngine - C++ Game Engine Library
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

#ifndef XENGINE_FRAMEGRAPHSETTINGS_HPP
#define XENGINE_FRAMEGRAPHSETTINGS_HPP

#include <string>

#define USER_PROPERTY(name) static const char *name = #name;

namespace xng {
    namespace FrameGraphSettings {
        // float, Range(0, inf) Render resolution is backbuffer size * scale, if not defined the full backbuffer resolution is used, the rendered images are projected to fit the backbuffer resolution.
        USER_PROPERTY(RENDER_SCALE)

        // int, the number of sub samples per pixel (MSAA) to use when forward rendering, if not defined the back buffer sample count is used.
        USER_PROPERTY(RENDER_SAMPLES)

        // Vec2i, The resolution of the shadow maps
        USER_PROPERTY(SHADOW_MAPPING_RESOLUTION)
    }
}

#endif //XENGINE_FRAMEGRAPHSETTINGS_HPP
