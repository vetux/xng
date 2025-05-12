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

#ifndef XENGINE_FRAMEGRAPH2_HPP
#define XENGINE_FRAMEGRAPH2_HPP

#include <vector>
#include <functional>

#include "xng/render/graph2/buffer/fgvertexbuffer.hpp"
#include "xng/render/graph2/buffer/fgindexbuffer.hpp"
#include "xng/render/graph2/buffer/fgtexturebuffer.hpp"

#include "xng/render/graph2/shader/fgshader.hpp"

#include "xng/render/graph2/fgpass.hpp"

namespace xng  {
    class FGContext;

    struct FGGraph{
        std::vector<FGPass> passes;

        std::vector<FGVertexBuffer> vertexBuffers;
        std::vector<FGIndexBuffer> indexBuffers;
        std::vector<FGTextureBuffer> textureBuffers;

        std::vector<FGShader> shaders;
    };
}

#endif //XENGINE_FRAMEGRAPH2_HPP
