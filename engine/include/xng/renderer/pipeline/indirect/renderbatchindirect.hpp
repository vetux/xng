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

#ifndef XENGINE_RENDERBATCHINDIRECT_HPP
#define XENGINE_RENDERBATCHINDIRECT_HPP

#include "xng/renderer/pipeline/renderbatch.hpp"

namespace xng {
    class RenderBatchIndirect final : public RenderBatch {
    public:
        ~RenderBatchIndirect() override = default;

        DrawID add(const RenderObjectHandle<RenderTransform> &transform,
                   const RenderObjectHandle<RenderMaterial> &material,
                   const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                   bool receiveShadows,
                   int sortPriority) override {
        }

        void remove(DrawID id) override {
        }

        void setPointLights(const std::vector<RenderObjectHandle<RenderPointLight> > &lights) override;

        void setDirectionalLights(const std::vector<RenderObjectHandle<RenderDirectionalLight> > &lights) override;

        void setSpotLights(const std::vector<RenderObjectHandle<RenderSpotLight> > &lights) override;

        void setCamera(const Vec3f &position, const Mat4f &view, const Mat4f &projection) override;
    };
}

#endif //XENGINE_RENDERBATCHINDIRECT_HPP
