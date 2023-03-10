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

#include "xng/gui/recttransform.hpp"

namespace xng {
    static RectTransform getRectRecursive(int rect,
                                          const std::map<int, RectTransform> &rects,
                                          const std::map<int, int> &parentMapping) {
        auto &r = rects.at(rect);
        RectTransform ret;
        ret.alignment = r.alignment;
        ret.center = r.center;
        ret.size = r.size;
        if (parentMapping.find(rect) != parentMapping.end()) {
            auto parentRect = getRectRecursive(parentMapping.at(rect), rects, parentMapping);
            ret.position += parentRect.position;
            ret.position += r.position;
            ret.position += RectTransform::getAlignmentOffset(r.size, parentRect.size, r.alignment);
        } else {
            ret.position = r.position;
        }
        return ret;
    }

    std::map<int, RectTransform>
            RectTransform::getAbsolute(const std::map<int, RectTransform> &rects, const std::map<int, int> &parentMapping) {
        std::map<int, RectTransform> ret;
        for (auto &pair: rects) {
            RectTransform rect = getRectRecursive(pair.first, rects, parentMapping);
            ret[pair.first] = rect;
        }
        return ret;
    }

    Vec2f RectTransform::getAlignmentOffset(const Vec2f &srcSize, const Vec2f &dstSize, RectTransform::Alignment align) {
        switch(align){
            case RECT_ALIGN_LEFT_TOP:
                return {};
            case RECT_ALIGN_CENTER_TOP:
                return {dstSize.x / 2 - srcSize.x / 2, 0};
            case RECT_ALIGN_RIGHT_TOP:
                return {dstSize.x - srcSize.x, 0};
            case RECT_ALIGN_LEFT_CENTER:
                return {0, dstSize.y / 2 - srcSize.y / 2};
            case RECT_ALIGN_CENTER_CENTER:
                return {dstSize.x / 2 - srcSize.x / 2, dstSize.y / 2 - srcSize.y / 2};
            case RECT_ALIGN_RIGHT_CENTER:
                return {dstSize.x - srcSize.x, dstSize.y / 2 - srcSize.y / 2};
            case RECT_ALIGN_LEFT_BOTTOM:
                return {0, dstSize.y - srcSize.y};
            case RECT_ALIGN_CENTER_BOTTOM:
                return {dstSize.x / 2 - srcSize.x / 2, dstSize.y - srcSize.y};
            case RECT_ALIGN_RIGHT_BOTTOM:
                return {dstSize.x - srcSize.x, dstSize.y - srcSize.y};
            default:
                throw std::runtime_error("Invalid alignment");
        }
    }
}