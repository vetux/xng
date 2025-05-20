/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_FGCONTEXT_HPP
#define XENGINE_FGCONTEXT_HPP

#include <cstdint>

#include "xng/render/graph2/shader/fgshadervalue.hpp"

#include "xng/render/graph2/fgdrawcall.hpp"

#include "xng/render/graph2/fgresource.hpp"

#include "xng/render/graph2/texture/fgtextureproperties.hpp"

namespace xng {
    class FGContext {
    public:
        // Data upload
        virtual void uploadBuffer(FGResource buffer, const uint8_t *ptr, size_t size) = 0;

        virtual void uploadTexture(FGResource texture,
                                   const uint8_t *ptr,
                                   size_t size,
                                   graph::FGCubeMapFace face/*,...*/) = 0;

        // Bindings
        virtual void bindVertexBuffer(FGResource buffer) = 0;
        virtual void bindIndexBuffer(FGResource buffer) = 0;

        virtual void bindInputTexture(std::string binding, FGResource texture) = 0;
        virtual void bindOutputTexture(std::string binding, FGResource texture) = 0;

        // Shader values (Shader Buffer, Push Constants, etc.)
        virtual void writeShaderValue(const std::string &name, int value) = 0;
        virtual int readShaderValue(const std::string &name) = 0;

        /**
         * Bind the given shader.
         *
         * @param shader
         */
        virtual void bindShader(FGResource shader) = 0;

        /**
         * Execute the draw call with the bound shaders and geometry data.
         *
         * @param call
         */
        virtual void draw(const FGDrawCall &call) = 0;
    };
}

#endif //XENGINE_FGCONTEXT_HPP
