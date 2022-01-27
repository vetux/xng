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

#ifndef XENGINE_GCONSTRUCTOR_HPP
#define XENGINE_GCONSTRUCTOR_HPP

#include "platform/graphics/renderdevice.hpp"

#include "render/deferred/gbuffer.hpp"

#include "asset/scene.hpp"
#include "asset/manager/assetrendermanager.hpp"

namespace xengine {
    class XENGINE_EXPORT GConstructor {
    public:
        explicit GConstructor(RenderDevice &device);

        /**
         * Update the geometry textures from the given scene data in the given geometry buffer.
         *
         * Users may override this function to define custom logic at the start of the pipeline.
         *
         * @param buffer
         * @param scene
         * @param assetRenderManager
         */
        virtual void create(GBuffer &buffer, Scene &scene, AssetRenderManager &assetRenderManager);

    protected:
        RenderDevice &renderDevice;

        ShaderSource vs;
        ShaderSource fs;

        std::unique_ptr<ShaderProgram> shader;
        std::unique_ptr<TextureBuffer> defaultTexture; //1 pixel texture with value (0, 0, 0, 0)
    };
}

#endif //XENGINE_GCONSTRUCTOR_HPP
