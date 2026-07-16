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
        enum Property : RenderPipelineMaterial::PropertyID {
            PAINT_COLOR = 0,
            PAINT_HAS_TEXTURE,
            PAINT_MIX,
            PAINT_MIX_COLOR,
        };

        enum Texture : RenderPipelineMaterial::TextureID {
            PAINT_TEXTURE,
        };

        static RenderPipeline::MaterialLayout getLayout() {
            RenderPipeline::MaterialLayout ret;
            ret.properties[PAINT_COLOR] = rg::ShaderPrimitiveType::vec4();
            ret.properties[PAINT_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();
            ret.properties[PAINT_MIX] = rg::ShaderPrimitiveType::vec4();
            ret.properties[PAINT_MIX_COLOR] = rg::ShaderPrimitiveType::vec4();
            ret.textures.insert(PAINT_TEXTURE);
            return ret;
        }
    };
}

#endif //XENGINE_MATERIALCANVAS_HPP
