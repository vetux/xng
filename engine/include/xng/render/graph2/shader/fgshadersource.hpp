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

#include <unordered_map>

#include "xng/render/graph2/shader/fgattributelayout.hpp"
#include "xng/render/graph2/shader/fgshadernode.hpp"
#include "xng/render/graph2/texture/fgtexture.hpp"
#include "xng/render/graph2/shader/fgshadervalue.hpp"
#include "xng/render/graph2/shader/fgshaderbuffer.hpp"

namespace xng {
    struct FGShaderBuffer;

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

        std::unordered_map<std::string, FGShaderValue> parameters;
        std::unordered_map<std::string, FGShaderBuffer> buffers;
        std::unordered_map<std::string, FGTexture> textures;

        std::vector<std::shared_ptr<FGShaderNode> > nodes;

        FGShaderSource() = default;

        FGShaderSource(const ShaderStage stage,
                       const FGAttributeLayout &input_layout,
                       const FGAttributeLayout &output_layout,
                       const std::unordered_map<std::string, FGShaderValue> &parameters,
                       const std::unordered_map<std::string, FGShaderBuffer> &buffers,
                       const std::unordered_map<std::string, FGTexture> &textures,
                       const std::vector<std::shared_ptr<FGShaderNode> > &nodes)
            : stage(stage),
              inputLayout(input_layout),
              outputLayout(output_layout),
              parameters(parameters),
              buffers(buffers),
              textures(textures),
              nodes(nodes) {
        }
    };
}
#endif //XENGINE_FGSHADERSOURCE_HPP
