/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_COMMAND_HPP
#define XENGINE_COMMAND_HPP

#include <utility>

#include "xng/asset/color.hpp"

#include "xng/math/vector2.hpp"
#include "xng/math/vector3.hpp"

#include "xng/gpu/textureproperties.hpp"
#include "xng/gpu/rendertargetattachment.hpp"
#include "xng/gpu/drawcall.hpp"
#include "xng/gpu/shaderresource.hpp"
#include "xng/gpu/commanddata.hpp"

#include "xng/shader/shaderstage.hpp"

namespace xng {
    struct Command {
        enum Type : int {
            NONE = 0,
            BLIT_COLOR,
            BLIT_DEPTH,
            BLIT_STENCIL,
            BEGIN_PASS,
            END_PASS,
            CLEAR_COLOR,
            CLEAR_DEPTH,
            SET_VIEWPORT,
            DRAW_ARRAY,
            DRAW_INDEXED,
            DRAW_ARRAY_INSTANCED,
            DRAW_INDEXED_INSTANCED,
            DRAW_ARRAY_MULTI,
            DRAW_INDEXED_MULTI,
            DRAW_INDEXED_BASE_VERTEX,
            DRAW_INDEXED_INSTANCED_BASE_VERTEX,
            DRAW_INDEXED_MULTI_BASE_VERTEX,
            BIND_PIPELINE,
            BIND_SHADER_RESOURCES,
            BIND_VERTEX_ARRAY_OBJECT,
            COPY_TEXTURE_ARRAY,
            COPY_TEXTURE,
            COPY_INDEX_BUFFER,
            COPY_VERTEX_BUFFER,
            COPY_SHADER_STORAGE_BUFFER,
            COPY_SHADER_UNIFORM_BUFFER,
            COMPUTE_BIND_PIPELINE,
            COMPUTE_BIND_DATA,
            COMPUTE_EXECUTE,
        } type{};

        CommandData data;

        Command() = default;

        Command(Type type, CommandData data)
                : type(type), data(std::move(data)) {}
    };
}
#endif //XENGINE_COMMAND_HPP
