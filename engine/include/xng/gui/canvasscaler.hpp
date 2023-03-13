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

#ifndef XENGINE_CANVASSCALER_HPP
#define XENGINE_CANVASSCALER_HPP

#include "xng/gui/recttransform.hpp"

namespace xng {
    class CanvasScaler {
    public:
        /**
         * Scale the given rect transform using reference resolution scaling which works identically to unity canvas reference scaling.
         *
         * @param absRect
         * @param referenceResolution
         * @param screenSize
         * @param fitWidth
         * @return
         */
        static RectTransform scaleReferenceResolution(const RectTransform &absRect,
                                                      const Vec2f &referenceResolution,
                                                      const Vec2f &screenSize,
                                                      float fitWidth);
    };
}

#endif //XENGINE_CANVASSCALER_HPP
