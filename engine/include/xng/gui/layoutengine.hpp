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

#ifndef XENGINE_LAYOUTENGINE_HPP
#define XENGINE_LAYOUTENGINE_HPP

#include <map>

#include "xng/gui/recttransform.hpp"

namespace xng {
    class LayoutEngine {
    public:
        /**
         * Generate the absolute values used for drawing with the Renderer2D of the given rect transform hierarchy.
         *
         * @return
         */
        static std::map<int, RectTransform> getAbsolute(const std::map<int, RectTransform> &rects,
                                                        const std::map<int, int> &parentMapping);

        /**
         * Generate the absolute values used for drawing with the Renderer2D of the given rect transform hierarchy
         * and apply reference scaling (CanvasScaler::scaleReferenceResolution).
         *
         * @return
         */
        static std::map<int, RectTransform> getAbsoluteReferenceScaled(const std::map<int, RectTransform> &rects,
                                                                       const std::map<int, int> &parentMapping,
                                                                       const Vec2f &referenceResolution,
                                                                       const Vec2f &screenSize,
                                                                       float fitWidth);

        /**
         * Generate the offset for the given alignment relative to the given size
         *
         * offset = eg. RECT_ALIGN_CENTER_CENTER dstSize / 2 - srcSize / 2
         *
         * @param v
         * @return
         */
        static Vec2f getAlignmentOffset(const Vec2f &srcSize, const Vec2f &dstSize, RectTransform::Alignment v);
    };
}

#endif //XENGINE_LAYOUTENGINE_HPP
