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

#ifndef XENGINE_RENDERER_HPP
#define XENGINE_RENDERER_HPP

#include "xng/renderer/renderscene.hpp"

namespace xng {
    struct DrawState {
        RenderObjectHandle<RenderCamera> camera;
        std::vector<RenderObjectHandle<RenderModel> > models;
    };

    /**
     * The renderer dispatches draw instructions to a user-supplied list of passes.
     *
     * There is no selective lights drawing for now.
     * This means any light object present in a scene contributes to the lighting.
     */
    class Renderer {
    public:
        void draw(const RenderCamera &camera, const std::vector<std::reference_wrapper<RenderModel> > &models);

    private:
        RenderScene scene;
    };
}

#endif //XENGINE_RENDERER_HPP
