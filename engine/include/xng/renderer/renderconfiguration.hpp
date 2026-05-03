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

#ifndef XENGINE_RENDERCONFIGURATION_HPP
#define XENGINE_RENDERCONFIGURATION_HPP

#include "xng/renderer/renderscene.hpp"
#include "xng/renderer/canvas/canvas.hpp"

namespace xng {
    struct RenderConfiguration {
        void setGamma(const float value) { gamma = value; }

        float getGamma() const { return gamma; }

        void setRenderScale(const float scale) { renderScale = scale; }

        float getRenderScale() const { return renderScale; }

        void setPointShadowResolution(const int resolution) { pointShadowResolution = resolution; }

        int getPointShadowResolution() const { return pointShadowResolution; }

        void setSpotShadowResolution(const int resolution) { spotShadowResolution = resolution; }

        int getSpotShadowResolution() const { return spotShadowResolution; }

        void setDirectionalShadowResolution(const int resolution) { directionalShadowResolution = resolution; }

        int getDirectionalShadowResolution() const { return directionalShadowResolution; }

        void setCompositingClearColor(const ColorRGBA &color) { compositingClearColor = color; }

        const ColorRGBA &getCompositingClearColor() const { return compositingClearColor; }

        float renderScale = 1.0f;

        int pointShadowResolution = 1024;
        int spotShadowResolution = 1024;
        int directionalShadowResolution = 1024;

        ColorRGBA compositingClearColor = ColorRGBA::white();
        float gamma = 1;

        // IBL texture size configuration
        Vec2i iblCubemapSize{512, 512};
        Vec2i iblPrefilterSize{128, 128};
        Vec2i iblIrradianceSize{128, 128};
        Vec2i iblBRDFSize{512, 512};
    };
}

#endif //XENGINE_RENDERCONFIGURATION_HPP
