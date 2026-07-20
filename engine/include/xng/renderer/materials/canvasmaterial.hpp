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

#ifndef XENGINE_CANVASMATERIAL_HPP
#define XENGINE_CANVASMATERIAL_HPP

#include "xng/renderer/pipeline/renderpipeline.hpp"

namespace xng {
    class CanvasMaterial {
    public:
        enum Property : RenderPipelineMaterial::PropertyID {
            PAINT_COLOR = 0,
            PAINT_MIX,
            PAINT_HAS_TEXTURE,
        };

        enum Texture : RenderPipelineMaterial::TextureID {
            PAINT_TEXTURE,
        };

        static RenderPipeline::MaterialLayout getLayout() {
            RenderPipeline::MaterialLayout ret;
            ret.properties[PAINT_COLOR] = rg::ShaderPrimitiveType::vec4();
            ret.properties[PAINT_MIX] = rg::ShaderPrimitiveType::vec4();
            ret.properties[PAINT_HAS_TEXTURE] = rg::ShaderPrimitiveType::Bool();
            ret.textures.insert(PAINT_TEXTURE);
            return ret;
        }

        CanvasMaterial() {
            properties[PAINT_COLOR] = rg::ShaderPrimitive(Vec4f(1.0f));
            properties[PAINT_MIX] = rg::ShaderPrimitive(Vec4f(0.0f));
            properties[PAINT_HAS_TEXTURE] = rg::ShaderPrimitive(false);
        }

        void setColor(const ColorRGBA &color) {
            properties[PAINT_COLOR] = rg::ShaderPrimitive(color.divide());
        }

        void setMix(const Vec4f &mix) {
            properties[PAINT_MIX] = rg::ShaderPrimitive(mix);
        }

        void setTexture(RenderObjectHandle<RenderTexture> texture, const SamplingProperties &samplingProperties) {
            textures[PAINT_TEXTURE] = {std::move(texture), samplingProperties};
            properties[PAINT_HAS_TEXTURE] = rg::ShaderPrimitive(true);
        }

        const std::unordered_map<RenderPipelineMaterial::PropertyID, rg::ShaderPrimitive> &getProperties() const {
            return properties;
        }

        const std::unordered_map<RenderPipelineMaterial::TextureID, RenderPipelineMaterial::TextureSampler> &
        getTextures() const {
            return textures;
        }

    private:
        std::unordered_map<RenderPipelineMaterial::PropertyID, rg::ShaderPrimitive> properties;
        std::unordered_map<RenderPipelineMaterial::TextureID, RenderPipelineMaterial::TextureSampler> textures;
    };
}

#endif //XENGINE_CANVASMATERIAL_HPP
