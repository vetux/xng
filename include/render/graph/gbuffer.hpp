/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_GBUFFER_HPP
#define XENGINE_GBUFFER_HPP

#include <memory>

#include "render/platform/renderdevice.hpp"
#include "render/platform/texturebuffer.hpp"

namespace xengine {
    class XENGINE_EXPORT GBuffer {
    public:
        /**
         * X are currently unused components.
         */
        enum GTexture : int {
            POSITION = 0, // RGBA32F : World Space Position xyz, w = X
            NORMAL = 1, // RGBA32F : Vertex Normal xyz, w = X
            TANGENT = 2, // RGBA32F : Vertex Tangent xyz, w = X
            TEXTURE_NORMAL = 3, // RGBA32F : Texture Normal in tangent space or a vector of zero length if no texture normals, xyz, w = X
            DIFFUSE = 4, // RGBA : The diffuse color value with alpha in the w channel.
            AMBIENT = 5, // RGBA : The ambient color value xyz, w = X
            SPECULAR = 6, // RGBA : The specular color value xyz, w = X
            ID_SHININESS = 7, // RGBA32I : .x = ID, .y = SHININESS, .z = X, .w = X
            DEPTH = 8, // DEPTH_STENCIL : The depth value in the x component
            GEOMETRY_TEXTURE_BEGIN = POSITION,
            GEOMETRY_TEXTURE_END = DEPTH
        };

        GBuffer() = default;

        explicit GBuffer(RenderDevice &device, Vec2i size = {640, 320}, int samples = 4);

        ~GBuffer();

        GBuffer(const GBuffer &other) = default;

        GBuffer(GBuffer &&other) = default;

        GBuffer &operator=(const GBuffer &other) = default;

        GBuffer &operator=(GBuffer &&other) = default;

        Vec2i getSize() const;

        int getSamples() const;

        /**
         * Get the texture buffer object for the given type.
         *
         * @param type
         * @return
         */
        TextureBuffer &getTexture(GTexture type) const;

    private:
        Vec2i size = {1, 1}; //The size of the geometry textures
        int samples = 1; //The number of msaa samples to use for geometry textures, all geometry textures are TEXTURE_2D_MULTISAMPLE

        std::map<GTexture, std::shared_ptr<TextureBuffer>> textures;
    };
}

#endif //XENGINE_GBUFFER_HPP
