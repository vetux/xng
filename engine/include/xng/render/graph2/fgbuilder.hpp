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

#ifndef XENGINE_FGBUILDER_HPP
#define XENGINE_FGBUILDER_HPP

#include <string>
#include <functional>

#include "xng/render/graph2/fggraph.hpp"
#include "xng/render/graph2/fgcontext.hpp"
#include "xng/render/graph2/shader/fgshader.hpp"

#include "xng/render/graph2/buffer/fgindexbuffer.hpp"
#include "xng/render/graph2/buffer/fgvertexbuffer.hpp"
#include "xng/render/graph2/buffer/fgtexturebuffer.hpp"

namespace xng {
    class FGBuilder {
    public:
        typedef int PassHandle;

        FGTextureBuffer createTextureBuffer();
        FGVertexBuffer createVertexBuffer();
        FGIndexBuffer createIndexBuffer();

        FGShaderHandle createShader(const FGShader &shader);

        PassHandle addPass(const std::string &name, std::function<void(FGContext &)> pass);

        void read(PassHandle pass, const FGBuffer& resource);
        void write(PassHandle pass, const FGBuffer& resource);
        void readWrite(PassHandle pass, const FGBuffer& resource);

        // Cross-Pass Dependencies
        void declareInput(PassHandle pass, const std::string &name);
        void declareOutput(PassHandle pass, const std::string &name, const FGBuffer &resource);

        FGGraph build();
    };
}

#endif //XENGINE_FGBUILDER_HPP
