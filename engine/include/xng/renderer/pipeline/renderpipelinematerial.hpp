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

#ifndef XENGINE_RENDERPIPELINEMATERIAL_HPP
#define XENGINE_RENDERPIPELINEMATERIAL_HPP

#include <cstdint>
#include <variant>

#include "xng/renderer/objects/rendertexture.hpp"

namespace xng {
    /**
     * A pipeline material represents the per-draw data.
     * The format of a material is dynamically defined when instantiating a pipeline.
     */
    class RenderPipelineMaterial {
    public:
        typedef size_t PropertyID;

        typedef size_t TextureID;

        struct TextureSampler {
            RenderObjectHandle<RenderTexture> texture;
            SamplingProperties samplingProperties;

            TextureSampler() = default;

            TextureSampler(const RenderObjectHandle<RenderTexture> &texture,
                           const SamplingProperties &samplingProperties)
                : texture(texture),
                  samplingProperties(samplingProperties) {
            }

            TextureSampler(const TextureSampler &other) = default;

            TextureSampler &operator=(const TextureSampler &other) = default;

            TextureSampler(TextureSampler &&other) noexcept {
                texture = other.texture;
                samplingProperties = std::move(other.samplingProperties);
            }

            TextureSampler &operator=(TextureSampler &&other) noexcept {
                if (this == &other) {
                    return *this;
                }
                texture = other.texture;
                samplingProperties = std::move(other.samplingProperties);
                return *this;
            }
        };

        virtual ~RenderPipelineMaterial() = default;

        virtual void update(const std::unordered_map<PropertyID, rg::ShaderPrimitive> &properties,
                            const std::unordered_map<TextureID, TextureSampler> &textures) = 0;

        virtual bool isUploadComplete() = 0;

        virtual void flush() = 0;
    };
}

#endif //XENGINE_RENDERPIPELINEMATERIAL_HPP
