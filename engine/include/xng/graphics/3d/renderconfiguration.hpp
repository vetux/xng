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

#ifndef XENGINE_RENDERCONFIGURATION_HPP
#define XENGINE_RENDERCONFIGURATION_HPP

#include "xng/graphics/renderscene.hpp"
#include "xng/graphics/2d/canvas.hpp"

namespace xng {
    class RenderConfiguration {
    public:
        void setRenderScale(const float scale) {
            renderScale = scale;
        }

        float getRenderScale() const {
            return renderScale;
        }

        void setScene(const RenderScene &scene) { renderScene = scene; }

        const RenderScene &getScene() const { return renderScene; }

        void setCanvases(const std::vector<Canvas> &canvases) { renderCanvases = canvases; }

        const std::vector<Canvas> &getCanvases() const { return renderCanvases; }

    private:
        float renderScale = 1.0f;
        RenderScene renderScene;

        std::vector<Canvas> renderCanvases;
    };
}

#endif //XENGINE_RENDERCONFIGURATION_HPP
