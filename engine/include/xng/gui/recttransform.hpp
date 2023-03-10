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

#ifndef XENGINE_RECTTRANSFORM_HPP
#define XENGINE_RECTTRANSFORM_HPP

#include <map>

#include "xng/math/vector2.hpp"

#include "xng/io/messageable.hpp"

namespace xng {
    struct RectTransform : public Messageable {
        enum Alignment : int {
            RECT_ALIGN_LEFT_TOP,
            RECT_ALIGN_CENTER_TOP,
            RECT_ALIGN_RIGHT_TOP,
            RECT_ALIGN_LEFT_CENTER,
            RECT_ALIGN_CENTER_CENTER,
            RECT_ALIGN_RIGHT_CENTER,
            RECT_ALIGN_LEFT_BOTTOM,
            RECT_ALIGN_CENTER_BOTTOM,
            RECT_ALIGN_RIGHT_BOTTOM
        } alignment = RECT_ALIGN_LEFT_TOP;

        Vec2f position{};
        Vec2f size{};
        Vec2f center{};
        float rotation{};

        /**
         * Generate the rect transforms which will be used to draw with the Renderer2D.
         * The position will be the sum of all parent absolute positions + (position * (parent.size / size))
         * The size is unmodified
         * The rotation will be the sum of all parent rotations + rotation
         *
         * @return
         */
        static std::map<int, RectTransform> getAbsolute(const std::map<int, RectTransform> &rects,
                                                        const std::map<int, int> &parentMapping);

        /**
         * Generate the offset for the given alignment relative to the given size
         *
         * offset = eg. RECT_ALIGN_CENTER_CENTER dstSize / 2 - srcSize / 2
         *
         * @param v
         * @return
         */
        static Vec2f getAlignmentOffset(const Vec2f &srcSize, const Vec2f &dstSize, Alignment v);

        Messageable &operator<<(const Message &message) override {
            message.value("alignment", reinterpret_cast<int&>(alignment), static_cast<int>(RECT_ALIGN_LEFT_TOP));
            message.value("position", position);
            message.value("size", size);
            message.value("center", center);
            message.value("rotation", rotation);
            return *this;
        }

        Message &operator>>(Message &message) const override {
            message = Message(Message::DICTIONARY);
            alignment >> message["alignment"];
            position >> message["position"];
            size >> message["size"];
            center >> message["center"];
            rotation >> message["rotation"];
            return message;
        }
    };
}
#endif //XENGINE_RECTTRANSFORM_HPP
