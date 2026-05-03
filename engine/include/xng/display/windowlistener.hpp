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

#ifndef XENGINE_WINDOWLISTENER_HPP
#define XENGINE_WINDOWLISTENER_HPP

#include "xng/math/vector2.hpp"

namespace xng {
    class XENGINE_EXPORT WindowListener {
    public:
        virtual ~WindowListener() = default;

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
