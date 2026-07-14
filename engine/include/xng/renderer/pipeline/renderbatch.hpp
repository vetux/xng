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

#ifndef XENGINE_RENDERBATCH_HPP
#define XENGINE_RENDERBATCH_HPP

#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"
#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/rendertransform.hpp"
#include "xng/renderer/objects/rendermaterial.hpp"

namespace xng {
    /**
     * A render batch represents the scene split based on object properties.
     *
     * The pipeline manages persistent state per batch such as indirect draw buffers, shader storage buffers, etc.
     *
     * The renderer manages batches and transforms / directs user-supplied render objects to the
     * appropriate pipeline / batch.
     *
     * Ideally, with indirect draw the cpu only iterates batches and never models.
     *
     * The batch also must handle draw call sorting internally based on sort priority and distance to the camera.
     */
    class RenderBatch {
    public:
        typedef size_t DrawID;

        virtual ~RenderBatch() = default;

        virtual DrawID add(const RenderObjectHandle<RenderTransform> &transform,
                           const RenderObjectHandle<RenderMaterial> &material,
                           const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                           bool receiveShadows,
                           int sortPriority) = 0;

        virtual void remove(DrawID id) = 0;

        virtual void setPointLights(const std::vector<RenderObjectHandle<RenderPointLight> > &lights) = 0;

        virtual void setDirectionalLights(const std::vector<RenderObjectHandle<RenderDirectionalLight> >
            &lights) = 0;

        virtual void setSpotLights(const std::vector<RenderObjectHandle<RenderSpotLight> > &lights) = 0;

        virtual void setCamera(const Vec3f &position, const Mat4f &view, const Mat4f &projection) = 0;

        virtual void setGamma(float gamma) = 0;

        /**
         * The batch will sort draw calls by distance to the camera for draw calls with identical sortPriority when enabled.
         *
         * Otherwise, draw calls are sorted only by sortPriority.
         *
         * @param enable
         */
        virtual void setEnableDistanceSort(bool enable) = 0;
    };
}

#endif //XENGINE_RENDERBATCH_HPP
