/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#include "xng/gui/layoutengine.hpp"

#include "xng/gui/canvasscaler.hpp"

namespace xng {
    static RectTransform getRectRecursiveReferenceScale(int rect,
                                                        const std::map<int, RectTransform> &rects,
                                                        const std::map<int, int> &parentMapping,
                                                        const Vec2f &referenceResolution,
                                                        const Vec2f &screenSize,
                                                        float fitWidth) {
        auto &r = rects.at(rect);
        RectTransform ret;
        ret.alignment = r.alignment;
        ret.center = r.center;
        ret.size = r.size;
        ret.position = r.position;

        if (parentMapping.find(rect) != parentMapping.end()) {
            ret = CanvasScaler::scaleReferenceResolution(ret, referenceResolution, screenSize, fitWidth);

            auto parentRect = getRectRecursiveReferenceScale(parentMapping.at(rect),
                                                             rects,
                                                             parentMapping,
                                                             referenceResolution,
                                                             screenSize,
                                                             fitWidth);
            ret.position += parentRect.position;
            ret.position += LayoutEngine::getAlignmentOffset(ret.size, parentRect.size, ret.alignment);
        }
        return ret;
    }

    static RectTransform getRectRecursive(int rect,
                                          const std::map<int, RectTransform> &rects,
                                          const std::map<int, int> &parentMapping) {
        auto &r = rects.at(rect);
        RectTransform ret;
        ret.alignment = r.alignment;
        ret.center = r.center;
        ret.size = r.size;
        ret.position = r.position;

        if (parentMapping.find(rect) != parentMapping.end()) {
            auto parentRect = getRectRecursive(parentMapping.at(rect),
                                               rects,
                                               parentMapping);
            ret.position += parentRect.position;
            ret.position += LayoutEngine::getAlignmentOffset(ret.size, parentRect.size, ret.alignment);
        }
        return ret;
    }

    std::map<int, RectTransform> LayoutEngine::getAbsolute(const std::map<int, RectTransform> &rects,
                                                           const std::map<int, int> &parentMapping) {
        std::map<int, RectTransform> ret;
        for (auto &pair: rects) {
            RectTransform rect = getRectRecursive(pair.first,
                                                  rects,
                                                  parentMapping);
            ret[pair.first] = rect;
        }
        return ret;
    }

    std::map<int, RectTransform> LayoutEngine::getAbsoluteReferenceScaled(const std::map<int, RectTransform> &rects,
                                                                          const std::map<int, int> &parentMapping,
                                                                          const Vec2f &referenceResolution,
                                                                          const Vec2f &screenSize,
                                                                          float fitWidth) {
        std::map<int, RectTransform> ret;
        for (auto &pair: rects) {
            RectTransform rect = getRectRecursiveReferenceScale(pair.first,
                                                                rects,
                                                                parentMapping,
                                                                referenceResolution,
                                                                screenSize,
                                                                fitWidth);
            ret[pair.first] = rect;
        }
        return ret;
    }

    Vec2f LayoutEngine::getAlignmentOffset(const Vec2f &srcSize, const Vec2f &dstSize, RectTransform::Alignment align) {
        switch (align) {
            case RectTransform::RECT_ALIGN_LEFT_TOP:
                return {};
            case RectTransform::RECT_ALIGN_CENTER_TOP:
                return {dstSize.x / 2 - srcSize.x / 2, 0};
            case RectTransform::RECT_ALIGN_RIGHT_TOP:
                return {dstSize.x - srcSize.x, 0};
            case RectTransform::RECT_ALIGN_LEFT_CENTER:
                return {0, dstSize.y / 2 - srcSize.y / 2};
            case RectTransform::RECT_ALIGN_CENTER_CENTER:
                return {dstSize.x / 2 - srcSize.x / 2, dstSize.y / 2 - srcSize.y / 2};
            case RectTransform::RECT_ALIGN_RIGHT_CENTER:
                return {dstSize.x - srcSize.x, dstSize.y / 2 - srcSize.y / 2};
            case RectTransform::RECT_ALIGN_LEFT_BOTTOM:
                return {0, dstSize.y - srcSize.y};
            case RectTransform::RECT_ALIGN_CENTER_BOTTOM:
                return {dstSize.x / 2 - srcSize.x / 2, dstSize.y - srcSize.y};
            case RectTransform::RECT_ALIGN_RIGHT_BOTTOM:
                return {dstSize.x - srcSize.x, dstSize.y - srcSize.y};
            default:
                throw std::runtime_error("Invalid alignment");
        }
    }
}