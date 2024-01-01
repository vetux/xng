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

#ifndef XENGINE_SHADERSTAGE_HPP
#define XENGINE_SHADERSTAGE_HPP

#include "xng/io/message.hpp"

namespace xng {
    enum ShaderStage : int {
        VERTEX = 0,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION,
        GEOMETRY,
        FRAGMENT,
        COMPUTE,
        RAY_GENERATE,
        RAY_HIT_ANY,
        RAY_HIT_CLOSEST,
        RAY_MISS,
        RAY_INTERSECT
    };

    inline ShaderStage &operator<<(ShaderStage &stage, const Message &message) {
        stage = (ShaderStage)message.asInt();
        return stage;
    }

    inline Message &operator>>(const ShaderStage &stage, Message &message) {
        message = (int)stage;
        return message;
    }
}

#endif //XENGINE_SHADERSTAGE_HPP
