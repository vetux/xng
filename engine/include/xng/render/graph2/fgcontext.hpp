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

#ifndef XENGINE_FGCONTEXT_HPP
#define XENGINE_FGCONTEXT_HPP

#include <cstdint>

#include "xng/render/graph2/shader/fgshaderliteral.hpp"
#include "xng/render/graph2/fgdrawcall.hpp"
#include "xng/render/graph2/fgresource.hpp"
#include "xng/render/scene/image.hpp"

namespace xng {
    class FGContext {
    public:
        // Data upload
        virtual void uploadBuffer(FGResource buffer, const uint8_t *ptr, size_t size) = 0;

        virtual void uploadTexture(FGResource texture,
                                   const uint8_t *ptr,
                                   size_t size,
                                   FGColorFormat format,
                                   size_t index = 0,
                                   size_t mipMapLevel = 0,
                                   FGCubeMapFace face = POSITIVE_X) = 0;

        // Bindings
        virtual void bindVertexBuffer(FGResource buffer) = 0;

        virtual void bindIndexBuffer(FGResource buffer) = 0;

        /**
         * Fragment shader output texture.
         *
         * @param binding
         * @param texture
         * @param index
         * @param mipMapLevel
         * @param face
         */
        virtual void bindOutputTexture(size_t binding,
                                       FGResource texture,
                                       size_t index = 0,
                                       size_t mipMapLevel = 0,
                                       FGCubeMapFace face = POSITIVE_X) = 0;

        virtual void bindTextures(const std::unordered_map<std::string, FGResource> &textures);

        virtual void bindShaderBuffers(const std::unordered_map<std::string, FGResource> &buffers);

        /**
         * Set the shader parameters for the next draw call. (Implemented as Push Constants)
         *
         * Shader parameters are values that change frequently (Per Draw) and have a size limit.
         *
         * Shader Parameters are set per draw call and must be set again for later draw calls.
         *
         * @param parameters
         */
        virtual void setShaderParameters(const std::unordered_map<std::string, FGShaderLiteral> &parameters);

        /**
         * Bind the given shaders.
         *
         * The shaders must form a complete pipeline eg. (1 Vertex Shader and 1 Fragment Shader) or 1 compute shader
         *
         * @param shaders
         */
        virtual void bindShaders(const std::vector<FGResource> &shaders) = 0;

        /**
         * Execute the draw call/s with the bound shaders and geometry data.
         *
         * @param calls
         */
        virtual void draw(const std::vector<FGDrawCall> &calls) = 0;

        // Data download
        virtual FGShaderLiteral downloadShaderParameter(const std::string &name) = 0;

        virtual Image<ColorRGBA> downloadTexture(FGResource texture,
                                                 size_t index = 0,
                                                 size_t mipMapLevel = 0,
                                                 FGCubeMapFace face = POSITIVE_X) = 0;

        /**
         * Return the internal compiled source code of the supplied shader.
         *
         * For debugging purposes only.
         *
         * @param shader
         * @return
         */
        virtual std::string getShaderSource(FGResource shader) = 0;
    };
}

#endif //XENGINE_FGCONTEXT_HPP
