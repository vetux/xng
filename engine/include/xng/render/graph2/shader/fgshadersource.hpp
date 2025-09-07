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

#ifndef XENGINE_FGSHADERSOURCE_HPP
#define XENGINE_FGSHADERSOURCE_HPP

#include "xng/render/graph2/shader/fgattributelayout.hpp"
#include "xng/render/graph2/texture/fgtextureproperties.hpp"
#include "xng/render/graph2/shader/fgshadernode.hpp"

namespace xng {
    /**
     * The shader compiler generates shader source code from the data in FGShaderSource.
     */
    class FGShaderSource {
    public:
        enum ShaderStage {
            VERTEX,
            GEOMETRY,
            TESSELATION_CONTROL,
            TESSELATION_EVALUATION,
            FRAGMENT,
            COMPUTE,
        } stage;

        FGAttributeLayout inputLayout;
        FGAttributeLayout outputLayout;

        std::vector<std::shared_ptr<FGShaderNode> > nodes;

        //TODO: Add support for different parameter upload methods (Push Constants, Uniform buffers etc.)

        //TODO: Parameters / textures output format from the shader compiler must be defined somehow so the runtime knows how to upload the data.
        /**
         * The available shader parameters and their corresponding format
         */
        std::map<std::string, FGAttributeElement> parameters;

        /**
         * The available textures and their corresponding format
         */
        std::map<std::string, graph::FGTextureProperties> textures;
    };
}
#endif //XENGINE_FGSHADERSOURCE_HPP
