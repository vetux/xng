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

#ifndef XENGINE_WINDOWLISTENER_HPP
#define XENGINE_WINDOWLISTENER_HPP

#include "math/vector2.hpp"

namespace xng {
    class XENGINE_EXPORT WindowListener {
    public:
        virtual void onWindowClose() {};

        virtual void onWindowMove(Vec2i pos) {};

        virtual void onWindowResize(Vec2i size) {};

        virtual void onWindowRefresh() {};

        virtual void onWindowFocus(bool focused) {};

        virtual void onWindowMinimize() {};

        virtual void onWindowMaximize() {};

        virtual void onWindowContentScale(Vec2f scale) {}

        virtual void onFramebufferResize(Vec2i size) {}
    };
}

#endif //XENGINE_WINDOWLISTENER_HPP
