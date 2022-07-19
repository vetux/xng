/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef XENGINE_RECTTRANSFORM_HPP
#define XENGINE_RECTTRANSFORM_HPP

namespace xng {
    struct XENGINE_EXPORT RectTransform {
        bool enabled;
        enum Anchor {
            TOP_LEFT,
            TOP_CENTER,
            TOP_RIGHT,
            LEFT,
            CENTER,
            RIGHT,
            BOTTOM_LEFT,
            BOTTOM_CENTER,
            BOTTOM_RIGHT
        } anchor = TOP_LEFT;

        Rectf rect;
        float rotation;
        std::string parent;

        static Vec2f getOffset(Anchor anchor, Vec2f canvasSize) {
            switch(anchor)
            {
                default:
                case TOP_LEFT:
                    return {};
                case TOP_CENTER:
                    return {canvasSize.x / 2, 0};
                case TOP_RIGHT:
                    return {canvasSize.x, 0};
                case LEFT:
                    return {0, canvasSize.y / 2};
                case CENTER:
                    return {canvasSize.x / 2, canvasSize.y / 2};
                case RIGHT:
                    return {canvasSize.x, canvasSize.y / 2};
                case BOTTOM_LEFT:
                    return {0, canvasSize.y};
                case BOTTOM_CENTER:
                    return {canvasSize.x / 2, canvasSize.y};
                case BOTTOM_RIGHT:
                    return {canvasSize.x, canvasSize.y};
            }
        }
    };
}
#endif //XENGINE_RECTTRANSFORM_HPP
