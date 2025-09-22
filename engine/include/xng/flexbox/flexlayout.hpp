/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_FLEXLAYOUT_HPP
#define XENGINE_FLEXLAYOUT_HPP

#include "xng/io/messageable.hpp"

#include "xng/math/vector2.hpp"
#include "xng/math/vector4.hpp"

#include "xng/flexbox/flexdirection.hpp"
#include "xng/flexbox/flexwrap.hpp"
#include "xng/flexbox/flexjustify.hpp"
#include "xng/flexbox/flexalign.hpp"

namespace xng {
    struct FlexLayout final : Messageable {
        Vec2f size = Vec2f(0, 0);
        Vec2f sizePercent = Vec2f(0, 0);

        Vec2f minSize = Vec2f(0, 0);
        Vec2f minSizePercent = Vec2f(0, 0);

        Vec2f maxSize = Vec2f(0, 0);
        Vec2f maxSizePercent = Vec2f(0, 0);

        float aspectRatio = 0;

        unsigned int order = 0;

        float grow = 0;
        float shrink = 0;
        float flexBasis = 0;
        float flexBasisPercent = 0;

        FlexAlign alignSelf{};

        Vec4f margin;
        Vec4f marginPercent;

        Vec4f padding;
        Vec4f paddingPercent;

        Vec4f border;

        float columnGaps = 0;
        float columnGapsPercent = 0;

        float rowGaps = 0;
        float rowGapsPercent = 0;

        FlexDirection direction{};
        FlexWrap wrap{};
        FlexJustify justifyContent{};
        FlexAlign alignItems{};
        FlexAlign alignContent{};

        Messageable &operator<<(const Message &message) override {
            message.value("size", size);
            message.value("sizePercent", sizePercent);

            message.value("minSize", minSize);
            message.value("minSizePercent", minSizePercent);

            message.value("maxSize", maxSize);
            message.value("maxSizePercent", maxSizePercent);

            message.value("aspectRatio", aspectRatio);

            message.value("order", order);

            message.value("grow", grow);
            message.value("shrink", shrink);
            message.value("flexBasis", flexBasis);
            message.value("flexBasisPercent", flexBasisPercent);

            message.value("alignSelf", reinterpret_cast<int &>(alignSelf));

            message.value("margin", margin);
            message.value("marginPercent", marginPercent);

            message.value("padding", padding);
            message.value("paddingPercent", paddingPercent);

            message.value("border", border);

            message.value("columnGaps", columnGaps);
            message.value("columnGapsPercent", columnGapsPercent);

            message.value("rowGaps", rowGaps);
            message.value("rowGapsPercent", rowGapsPercent);

            message.value("direction", reinterpret_cast<int &>(direction));
            message.value("wrap", reinterpret_cast<int &>(wrap));
            message.value("justifyContent", reinterpret_cast<int &>(justifyContent));
            message.value("alignItems", reinterpret_cast<int &>(alignItems));
            message.value("alignContent", reinterpret_cast<int &>(alignContent));

            return *this;
        }

        Message &operator>>(Message &message) const override {
            size >> message["size"];
            sizePercent >> message["sizePercent"];

            minSize >> message["minSize"];
            minSizePercent >> message["minSizePercent"];

            maxSize >> message["maxSize"];
            maxSizePercent >> message["maxSizePercent"];

            aspectRatio >> message["aspectRatio"];

            order >> message["order"];

            grow >> message["grow"];
            shrink >> message["shrink"];
            flexBasis >> message["flexBasis"];
            flexBasisPercent >> message["flexBasisPercent"];

            alignSelf >> message["alignSelf"];

            margin >> message["margin"];
            marginPercent >> message["marginPercent"];

            padding >> message["padding"];
            paddingPercent >> message["paddingPercent"];

            border >> message["border"];

            columnGaps >> message["columnGaps"];
            columnGapsPercent >> message["columnGapsPercent"];

            rowGaps >> message["rowGaps"];
            rowGapsPercent >> message["rowGapsPercent"];

            direction >> message["direction"];
            wrap >> message["wrap"];
            justifyContent >> message["justifyContent"];
            alignItems >> message["alignItems"];
            alignContent >> message["alignContent"];
            return message;
        }
    };
}

#endif //XENGINE_FLEXLAYOUT_HPP
