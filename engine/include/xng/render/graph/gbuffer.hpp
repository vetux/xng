/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_GBUFFER_HPP
#define XENGINE_GBUFFER_HPP

#include <memory>

#include "framegraphresource.hpp"
#include "framegraphbuilder.hpp"
#include "framegraphpassresources.hpp"

#include "xng/math/vector2.hpp"

#include "xng/gpu/rendertarget.hpp"
#include "xng/gpu/texturebuffer.hpp"

namespace xng {
    class XENGINE_EXPORT GBuffer {
    public:
        enum GTexture : int {
            GEOMETRY_TEXTURE_POSITION = 0, // RGBA32F : World Space Position xyz, w = X
            GEOMETRY_TEXTURE_NORMAL = 1, // RGBA32F : Vertex or Texture Normal xyz, w = X
            GEOMETRY_TEXTURE_TANGENT = 2, // RGBA32F : Vertex Tangent xyz, w = X
            GEOMETRY_TEXTURE_ROUGHNESS_METALLIC_AO = 3, // RGBA32f : .x = pbr roughness or phong shininess, .y = pbr metallic, .z = pbr ambient occlusion
            GEOMETRY_TEXTURE_ALBEDO = 4, // RGBA : The pbr albedo or phong diffuse color value
            GEOMETRY_TEXTURE_AMBIENT = 5, // RGBA : The phong ambient color value
            GEOMETRY_TEXTURE_SPECULAR = 6, // RGBA : The phong specular color value
            GEOMETRY_TEXTURE_LIGHTMODEL_OBJECT = 7, // RGBA32I : .x = Lighting Model ID, .y = Object ID
            GEOMETRY_TEXTURE_DEPTH = 8, // DEPTH_STENCIL : The depth value in the x component
            GEOMETRY_TEXTURE_BEGIN = GEOMETRY_TEXTURE_POSITION,
            GEOMETRY_TEXTURE_END = GEOMETRY_TEXTURE_DEPTH
        };

        GBuffer() = default;

        explicit GBuffer(Vec2i size, int samples);

        ~GBuffer();

        GBuffer(const GBuffer &other) = default;

        GBuffer(GBuffer &&other) = default;

        GBuffer &operator=(const GBuffer &other) = default;

        GBuffer &operator=(GBuffer &&other) = default;

        Vec2i getSize() const;

        int getSamples() const;

        /**
         * Create the resource handles.
         *
         * @param builder
         */
        void setup(FrameGraphBuilder &builder);

        /**
         * Get the texture buffer object for the given type.
         *
         * @param type
         * @return
         */
        TextureBuffer &getTexture(GTexture type, FrameGraphPassResources &resources) const;

    private:
        Vec2i size = {1, 1}; //The size of the textures
        int samples = 1; //The number of msaa samples to use for geometry textures, all geometry textures are TEXTURE_2D_MULTISAMPLE

        std::map<GTexture, FrameGraphResource> textures;
    };
}

#endif //XENGINE_GBUFFER_HPP
