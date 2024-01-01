/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_FRAMEGRAPHRENDERER_HPP
#define XENGINE_FRAMEGRAPHRENDERER_HPP

#include "xng/render/scenerenderer.hpp"

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/graph/framegraphpipeline.hpp"
#include "xng/render/graph/framegraphruntime.hpp"

#include "xng/shader/shadercompiler.hpp"
#include "xng/shader/shaderdecompiler.hpp"

#include "xng/gpu/renderdevice.hpp"

namespace xng {
    class XENGINE_EXPORT FrameGraphRenderer : public SceneRenderer {
    public:
        explicit FrameGraphRenderer(std::unique_ptr<FrameGraphRuntime> runtime);

        void render(const Scene &scene) override;

        void setPipeline(const FrameGraphPipeline &v) {
            pipeline = v;
        }

        void setSettings(const SceneRendererSettings &value) override {
            settings = value;
        }

        SceneRendererSettings &getSettings() override {
            return settings;
        }

        const SceneRendererSettings &getSettings() const override {
            return settings;
        }

    private:
        FrameGraphPipeline pipeline;
        SceneRendererSettings settings;
        std::unique_ptr<FrameGraphRuntime> runtime;
        std::set<FrameGraphResource> persistentResources;
    };
}
#endif //XENGINE_FRAMEGRAPHRENDERER_HPP
