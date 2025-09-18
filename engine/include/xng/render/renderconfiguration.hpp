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

#ifndef XENGINE_RENDERCONFIGURATION_HPP
#define XENGINE_RENDERCONFIGURATION_HPP

#include "xng/render/renderscene.hpp"
#include "xng/math/vector2.hpp"

namespace xng {
    class RenderConfiguration {
    public:
        const Vec2i &getRenderResolution() const {
            if (renderResolution.length() == 0) {
                return framebufferResolution;
            } else {
                return renderResolution;
            }
        }

        const Vec2i &getFramebufferResolution() const { return framebufferResolution; }

        const RenderScene &getScene() const { return renderScene; }

        void setRenderResolution(const Vec2i &resolution) { this->renderResolution = resolution; }

        void setFramebufferResolution(const Vec2i &resolution) { this->framebufferResolution = resolution; }

        void setScene(const RenderScene &scene) { this->renderScene = scene; }

    private:
        Vec2i renderResolution;
        Vec2i framebufferResolution;
        RenderScene renderScene;
    };
}

#endif //XENGINE_RENDERCONFIGURATION_HPP
