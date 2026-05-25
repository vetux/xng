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

#ifndef XENGINE_RENDERVIEW_HPP
#define XENGINE_RENDERVIEW_HPP

#include "camera.hpp"
#include "xng/renderer/objects/rendermodel.hpp"
#include "xng/renderer/objects/rendercanvas.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"

namespace xng {
    /**
     * The list of objects to be compiled into a render scene.
     *
     * The object uploads might be in flight when compiling to a render scene.
     * The streams are committed every frame, and the scene only needs to be rebuilt when the stream buffers are reallocated.
     */
    struct RenderDrawList {
        Camera camera;

        std::vector<RenderObjectHandle<RenderModel> > models;

        std::vector<RenderObjectHandle<RenderDirectionalLight> > directionalLights;
        std::vector<RenderObjectHandle<RenderPointLight> > pointLights;
        std::vector<RenderObjectHandle<RenderSpotLight> > spotLights;

        /**
         * The canvases to draw.
         * The screen space canvases are presented to the output surface in declaration order.
         */
        std::vector<RenderObjectHandle<RenderCanvas> > canvases;
    };
}

#endif //XENGINE_RENDERVIEW_HPP
