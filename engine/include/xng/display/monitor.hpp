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

#ifndef XENGINE_MONITOR_HPP
#define XENGINE_MONITOR_HPP

#include <vector>
#include <string>

#include "videomode.hpp"
#include "xng/math/vector2.hpp"
#include "xng/math/rectangle.hpp"

namespace xng {
    class XENGINE_EXPORT Monitor {
    public:
        virtual ~Monitor() = default;

        virtual Vec2i getVirtualPosition() = 0;

        virtual Recti getWorkArea() = 0;

        virtual Vec2i getPhysicalSize() = 0;

        virtual Vec2f getContentScale() = 0;

        virtual std::string getName() = 0;

        virtual VideoMode getVideoMode() = 0;

        virtual std::vector<VideoMode> getSupportedVideoModes() = 0;
    };
}

#endif //XENGINE_MONITOR_HPP
