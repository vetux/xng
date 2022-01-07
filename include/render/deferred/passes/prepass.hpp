/**
 *  Mana - 3D Game Engine
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

#ifndef XENGINE_PREPASS_HPP
#define XENGINE_PREPASS_HPP

#include "render/deferred/renderpass.hpp"

namespace xengine {
    /**
     * The PrePass creates buffers which are accessed by the deferred passes.
     *
     * It executes a drawCall for each deferred command in the scene and stores the data in textures.
     */
    class XENGINE_EXPORT PrePass : public RenderPass {
    public:
        static const char *DEPTH;

        static const char *POSITION; // The world space position
        static const char *NORMAL; // The vertex normal
        static const char *TANGENT; // The vertex tangent
        static const char *TEXTURE_NORMAL; // The texture normal in tangent space
        static const char *DIFFUSE;
        static const char *AMBIENT;
        static const char *SPECULAR;
        static const char *SHININESS_ID; //x = shininess, y = id

        explicit PrePass(RenderDevice &device);

        ~PrePass() override;

        void prepareBuffer(GeometryBuffer &gBuffer) override;

        void render(GeometryBuffer &gBuffer, Scene &scene, AssetRenderManager &assetRenderManager) override;

    private:
        RenderDevice &renderDevice;

        ShaderSource vs;
        ShaderSource fs;

        std::unique_ptr<ShaderProgram> shader;

        std::unique_ptr<TextureBuffer> defaultTexture; //1 pixel texture with value (0, 0, 0, 0)
    };
}

#endif //XENGINE_PREPASS_HPP
