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

#ifndef XENGINE_RENDERPIPELINE_HPP
#define XENGINE_RENDERPIPELINE_HPP

#include "xng/renderer/objects/rendertexture.hpp"
#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"
#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/rendertransform.hpp"
#include "xng/renderer/objects/rendermaterial.hpp"

#include "xng/renderer/pipeline/rendershadercompiler.hpp"

namespace xng {
    /**
     * The RenderPipeline represents the rendering technique for RenderObjects.
     * This allows swapping the rendering technique at runtime (E.g. No Indirect draw on mobile platforms)
     *
     * Each pipeline manages persistent state such as indirect draw buffers, shader storage buffers, etc.
     *
     * The renderer transforms / directs user-supplied render objects to the appropriate pipeline.
     *
     * Ideally, with indirect draw the cpu only iterates batches and never models.
     *
     * The pipeline also must handle draw call sorting internally based on sort priority and distance to the camera.
     *
     * Essentially, the RenderPipeline interface extends the concept of a pipeline in the RenderGraph (Which represents
     * a hardware pipeline) to the full render path from scene to shader.
     */
    class RenderPipeline {
    public:
        typedef std::variant<rg::Attachment, RenderObjectHandle<RenderTexture> > Attachment;

        struct BufferBinding {
            rg::Resource<rg::Buffer> buffer;
            size_t offset{};
            size_t size{};
        };

        typedef size_t DrawID;

        virtual ~RenderPipeline() = default;

        virtual DrawID addDrawCall(const RenderObjectHandle<RenderTransform> &transform,
                                   const RenderObjectHandle<RenderMaterial> &material,
                                   const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                                   bool receiveShadows,
                                   int sortPriority) = 0;

        virtual void removeDrawCall(DrawID id) = 0;

        virtual void setPointLights(const std::vector<RenderObjectHandle<RenderPointLight> > &lights) = 0;

        virtual void setDirectionalLights(const std::vector<RenderObjectHandle<RenderDirectionalLight> > &lights) = 0;

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

        /**
         * Prepare the pipeline for execution. Must be called anytime the pipeline state changes before calling draw().
         *
         * @param graph The graph to record the pipeline preparation into.
         */
        virtual void prepare(rg::GraphBuilder &graph) = 0;

        /**
         * Execute the pipeline.
         *
         * @param graph The graph to record the draw invocations into.
         * @param shader The shader to use for drawing.
         * @param attachments The Attachments to bind. (Type / Format must match the format in the shader)
         * @param parameters Optional user-supplied shader parameters.
         * @param storageBuffers Optional user-supplied storage buffer bindings.
         * @param textureArrays Optional user-supplied texture bindings.
         */
        virtual void execute(rg::GraphBuilder &graph,
                             const RenderShader &shader,
                             std::vector<Attachment> attachments,
                             std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                             std::unordered_map<std::string, BufferBinding> storageBuffers,
                             std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays);
    };
}

#endif //XENGINE_RENDERPIPELINE_HPP
