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
#include "xng/render/graph2/fgresource.hpp"

#include "xng/render/graph2/shader/fgshadersource.hpp"
#include "xng/render/graph2/texture/fgtextureproperties.hpp"

namespace xng {
    class FGBuilder {
    public:
        typedef int PassHandle;

        FGResource createTexture(const graph::FGTextureProperties &properties);
        FGResource createVertexBuffer(size_t size);
        FGResource createIndexBuffer(size_t size);
        FGResource createShader(const FGShaderSource &shader);

        /**
         * Cross-Pass or Cross-Graph resource sharing.
         *
         * @param name
         * @return
         */
        FGResource importResource(const std::string &name);
        void exportResource(const std::string &name, FGResource resource);

        PassHandle addPass(const std::string &name, std::function<void(FGContext &)> pass);

        void read(PassHandle pass, FGResource resource);
        void write(PassHandle pass, FGResource resource);
        void readWrite(PassHandle pass, FGResource resource);

        FGGraph build();
    };
}

#endif //XENGINE_FGBUILDER_HPP
