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

#ifndef XENGINE_SCENERENDERER_HPP
#define XENGINE_SCENERENDERER_HPP

#include "asset/scene.hpp"
#include "gpu/rendertarget.hpp"

namespace xng {
    /**
     * A scene renderer produces an image from a given a scene and target.
     * The scene renderer depends on the gpu and display drivers.
     *
     * xng is designed to allow writing cross platform renderers through the gpu / display interface.
     *
     * However if a user wishes to implement a renderer without using the provided interfaces
     * or use a 3rd party renderer the drivers can be reimplemented as needed
     * and the renderer implemented as a SceneRenderer driver.
     */
    class XENGINE_EXPORT SceneRenderer : public Driver {
    public:
        virtual void render(RenderTarget &target, const Scene &scene) = 0;

        virtual void setRenderResolution(Vec2i res) = 0;

        virtual void setRenderSamples(int samples) = 0;

        std::type_index getBaseType() override {
            return typeid(SceneRenderer);
        }
    };
}
#endif //XENGINE_SCENERENDERER_HPP
