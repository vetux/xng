/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_TEXTUREBINDING_HPP
#define XENGINE_TEXTUREBINDING_HPP

#include "xng/rendergraph/resourceid.hpp"

#include "xng/rendergraph/resource/texture.hpp"

namespace xng::rendergraph {
    struct TextureBinding {
        enum Aspect {
            /**
             * Automatic:
             *  Runtime infers the aspect from the texture type.
             *  Color textures: Color
             *  Depth only texture: Depth
             *  Stencil only texture: Stencil
             *  Combined depth stencil texture: DepthStencil
             */
            Automatic,
            Color,
            Depth,
            Stencil,
            DepthStencil,
        };

        struct Range {
            /**
             * Specific mip range to bind.
             *
             * Shaders access mip levels inside this range transparently.
             * So sample(mipLevel = 0) samples mip level at 5 if baseMipLevel = 5.
             */
            unsigned int baseMipLevel = 0;
            unsigned int numMipLevels = 0; // 0 = All remaining layers

            /**
             * Specific array layer range to bind.
             *
             * Shaders access array layers inside this range transparently.
             * So sample(arrayLayer = 0) samples array layer at 3 if baseArrayLayer = 5.
             */
            unsigned int baseArrayLayer = 0;
            unsigned int numArrayLayers = 0; // 0 = All remaining layers

            Range() = default;

            Range(const unsigned int baseMipLevel,
                  const unsigned int numMipLevels,
                  const unsigned int baseArrayLayer,
                  const unsigned int numArrayLayers)
                : baseMipLevel(baseMipLevel),
                  numMipLevels(numMipLevels),
                  baseArrayLayer(baseArrayLayer),
                  numArrayLayers(numArrayLayers) {
            }
        };

        Resource<Texture> texture{};
        Range range{};
        Aspect aspect = Automatic;

        TextureBinding() = default;

        explicit TextureBinding(Resource<Texture> texture)
            : texture(std::move(texture)) {
        }

        TextureBinding(Resource<Texture> texture, const Range range, const Aspect aspect)
            : texture(std::move(texture)), range(range), aspect(aspect) {
        }
    };
}

#endif //XENGINE_TEXTUREBINDING_HPP
