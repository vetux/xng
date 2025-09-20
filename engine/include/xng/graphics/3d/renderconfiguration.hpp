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

#include "xng/graphics/renderscene.hpp"

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

        void setRenderBatches(const std::vector<RenderBatch2D> &batches) { renderBatches = batches; }

        const std::vector<RenderBatch2D> &getRenderBatches() const { return renderBatches; }

    private:
        float renderScale = 1.0f;
        RenderScene renderScene;
        std::vector<RenderBatch2D> renderBatches;
    };
}

#endif //XENGINE_RENDERCONFIGURATION_HPP
