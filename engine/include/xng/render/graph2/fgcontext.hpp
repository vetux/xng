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

#include "xng/render/graph2/buffer/fgvertexbuffer.hpp"
#include "xng/render/graph2/buffer/fgindexbuffer.hpp"
#include "xng/render/graph2/buffer/fgtexturebuffer.hpp"
#include "xng/render/graph2/buffer/fgbufferlayout.hpp"

#include "xng/render/graph2/shader/fgshadervalue.hpp"
#include "xng/render/graph2/shader/fgshader.hpp"

#include "xng/render/graph2/fgdrawcall.hpp"

namespace xng {
    class FGContext {
    public:
        // Import buffer from a previous pass
        virtual const FGBuffer &importBuffer(const std::string &name) = 0;

        // Data upload
        virtual void upload(const FGBuffer &buffer, const uint8_t *ptr, size_t size) = 0;

        // Bindings
        virtual void bindVertexBuffer(const FGVertexBuffer &buffer) = 0;
        virtual void bindIndexBuffer(const FGIndexBuffer &buffer) = 0;

        virtual void bindInputTexture(std::string binding, const FGTextureBuffer &texture) = 0;
        virtual void bindOutputTexture(std::string binding, const FGTextureBuffer &texture) = 0;

        // Shader values (Shader Buffer, Push Constants, etc.)
        virtual void writeShaderValue(const std::string &name, int value) = 0;
        virtual void readShaderValue(const std::string &name) = 0;

        /**
         * Bind the given shader.
         *
         * @param shader
         */
        virtual void bindShader(const FGShaderHandle &shader) = 0;

        /**
         * Execute the draw call with the bound shaders and geometry data.
         *
         * @param call
         */
        virtual void draw(const FGDrawCall &call) = 0;
    };
}

#endif //XENGINE_FGCONTEXT_HPP
