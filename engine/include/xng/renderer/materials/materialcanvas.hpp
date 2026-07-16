/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef XENGINE_MATERIALCANVAS_HPP
#define XENGINE_MATERIALCANVAS_HPP

#include "xng/renderer/pipeline/renderpipeline.hpp"

namespace xng {
    class MaterialCanvas {
    public:
        enum InstanceAttributes : RenderPipelineMaterial::AttributeID {
            PAINT_COLOR = 0,
            PAINT_TEXTURE,
            PAINT_HAS_TEXTURE,
            PAINT_MIX,
            PAINT_MIX_COLOR,
        };

        static RenderPipeline::MaterialAttributes getAttributes() {
            RenderPipeline::MaterialAttributes ret;
            ret[PAINT_COLOR] = RenderPipelineMaterial::AttributeType::primitive(rg::ShaderPrimitiveType::vec4());
            ret[PAINT_TEXTURE] = RenderPipelineMaterial::AttributeType::texture();
            ret[PAINT_HAS_TEXTURE] = RenderPipelineMaterial::AttributeType::primitive(rg::ShaderPrimitiveType::Bool());
            ret[PAINT_MIX] = RenderPipelineMaterial::AttributeType::primitive(rg::ShaderPrimitiveType::vec4());
            ret[PAINT_MIX_COLOR] = RenderPipelineMaterial::AttributeType::primitive(rg::ShaderPrimitiveType::vec4());
            return ret;
        }
    };
}

#endif //XENGINE_MATERIALCANVAS_HPP
