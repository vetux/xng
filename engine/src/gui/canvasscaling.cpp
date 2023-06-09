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

#include <algorithm>
#include "xng/gui/canvasscaler.hpp"

namespace xng {
    RectTransform CanvasScaler::scaleReferenceResolution(const RectTransform &absRect,
                                                         const Vec2f &referenceResolution,
                                                         const Vec2f &screenSize,
                                                         float fitWidth) {
        fitWidth = std::clamp(fitWidth, 0.0f, 1.0f);

        RectTransform ret = absRect;

        auto centerPos = ret.position;

        auto posScale = Vec2f(screenSize.x / referenceResolution.x, screenSize.y / referenceResolution.y);

        auto sizeScale = Vec2f(std::lerp(posScale.y, posScale.x, fitWidth));

        auto scaledSize = ret.size * sizeScale;

        auto scaledPos = centerPos * posScale;

        ret.position = scaledPos;
        ret.size = scaledSize;

        return ret;
    }
}
