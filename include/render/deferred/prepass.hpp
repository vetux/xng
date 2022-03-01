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

#ifndef XENGINE_PREPASS_HPP
#define XENGINE_PREPASS_HPP

#include "render/scene.hpp"
#include "render/deferred/gbuffer.hpp"

namespace xengine {
    class PrePass {
    public:
        explicit PrePass(RenderDevice &device);

        /**
         * Update the geometry textures from the given scene data.
         * This is also where skeletal animation matrices are applied.
         *
         * Users may override this method to define logic at the start of the pipeline.
         *
         * @param scene
         * @param buffer
         * @param assetRenderManager
         */
        virtual void update(Scene &scene, GBuffer &buffer);

    private:
        Renderer &ren;

        ShaderSource vs;
        ShaderSource fs;

        std::unique_ptr<ShaderProgram> shader;
        std::unique_ptr<TextureBuffer> defaultTexture; //1 pixel texture with value (0, 0, 0, 0)
    };
}

#endif //XENGINE_PREPASS_HPP
