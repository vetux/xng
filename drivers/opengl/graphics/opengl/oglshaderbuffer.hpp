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

#ifndef XENGINE_OGLSHADERBUFFER_HPP
#define XENGINE_OGLSHADERBUFFER_HPP

#include "graphics/opengl/oglbuildmacro.hpp"

#warning NOT IMPLEMENTED

namespace xengine::opengl {
    class OPENGL_TYPENAME(ShaderBuffer) : public ShaderBuffer OPENGL_INHERIT {
    public:
        ShaderBufferDesc desc;

        explicit OPENGL_TYPENAME(ShaderBuffer)(ShaderBufferDesc desc)
                : desc(desc) {
            initialize();
        }

        void pinGpuMemory() override {}

        void unpinGpuMemory() override {}

        const ShaderBufferDesc &getDescription() override {
            return desc;
        }

        void upload(const uint8_t *data, size_t size) override {
        }

        OPENGL_MEMBERS
    };
}

#endif //XENGINE_OGLSHADERBUFFER_HPP
