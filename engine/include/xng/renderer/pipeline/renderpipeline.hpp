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
#include "xng/renderer/objects/rendermesh.hpp"

#include "xng/renderer/pipeline/renderpipelinecompiler.hpp"
#include "xng/renderer/pipeline/renderpipelinematerial.hpp"
#include "xng/renderer/pipeline/renderpipelinetransform.hpp"

namespace xng {
    /**
     * The RenderPipeline defines a fixed-function-like pipeline interface for drawing high-level scene objects.
     *
     * Each pipeline is owned by a scene and references objects in that scene via ID.
     *
     * The pipeline internally automates instancing based on mesh references and may perform more advanced drawing
     * techniques such as indirect drawing where the pipeline will never iterate the draws on cpu.
     *
     * The pipeline handles culling and optionally distance-based sorting.
     *
     * It will for now have 2 implementations:
     * - High-performance indirect gpu-driven implementation
     * - Array / instanced-based implementation for platforms where indirect draw is not available / stable.
     *
     * Materials define the data available per draw instance and are dynamically configurable at runtime,
     * which allows users to write shaders against custom material data while still using the full optimized
     * batched render path.
     *
     * Users can bind other global data via custom bindings such as light arrays or global configuration data like gamma
     * the pipeline only manages the link between materials and meshes.
     */
    class RenderPipeline {
    public:
        /**
         * An attachment can either be a RenderGraph attachment or a RenderTexture
         */
        typedef std::variant<rg::Attachment, RenderObjectHandle<RenderTexture> > Attachment;

        struct MaterialLayout {
            std::unordered_map<RenderPipelineMaterial::PropertyID, rg::ShaderPrimitiveType> properties;
            std::unordered_set<RenderPipelineMaterial::TextureID> textures;

            bool operator==(const MaterialLayout &other) const {
                return properties == other.properties && textures == other.textures;
            }
        };

        struct BufferBinding {
            rg::Resource<rg::Buffer> buffer;
            size_t offset{};
            size_t size{};
        };

        typedef size_t DrawID;

        virtual ~RenderPipeline() = default;

        /**
         * @return The material properties and textures available in this pipeline.
         */
        virtual const MaterialLayout &getMaterialLayout();

        /**
         * @return The shader compiler for this pipeline.
         */
        virtual RenderPipelineCompiler &getCompiler();

        virtual std::shared_ptr<RenderPipelineTransform> createTransform();

        virtual std::shared_ptr<RenderPipelineMaterial> createMaterial();

        virtual DrawID addDrawCall(std::shared_ptr<RenderPipelineTransform> transform,
                                   std::shared_ptr<RenderPipelineMaterial> material,
                                   const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                                   int sortPriority);

        virtual DrawID addDrawCall(std::shared_ptr<RenderPipelineTransform> transform,
                                   const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                                   int sortPriority);

        virtual void removeDrawCall(DrawID id) = 0;

        virtual void setCamera(const Vec3f &position, const Mat4f &view, const Mat4f &projection) = 0;

        /**
         * The pipeline will sort draw calls by distance to the camera for draw calls with identical sortPriority when enabled.
         *
         * Otherwise, draw calls are sorted only by sortPriority.
         *
         * @param enable
         */
        virtual void setEnableDistanceSort(bool enable) = 0;

        /**
         * The pipeline will cull draw calls in prepare() if enabled.
         *
         * Currently, this is just a stub because for proper draw culling mesh volumes are required.
         * This will be properly implemented with meshlet support.
         *
         * @param enable Whether to enable draw call culling.
         */
        virtual void setEnableDrawCulling(bool enable) = 0;

        /**
         * Commit the internal buffers of the pipeline.
         *
         * @param graph
         * @param streamerQueue
         */
        virtual void commit(rg::GraphBuilder &graph, StreamerQueue &streamerQueue) = 0;

        /**
         * Prepare the pipeline for execution. Must be called anytime the pipeline state changes before calling draw().
         *
         * This will record the prepass in the indirect pipeline.
         *
         * @param graph The graph to record the pipeline preparation into.
         */
        virtual void prepare(rg::GraphBuilder &graph) = 0;

        /**
         * Execute the pipeline.
         *
         * @param graph The graph to record the draw invocations into.
         * @param shader The shader to use for drawing.
         * @param viewport The viewport to set.
         * @param attachments The Attachments to bind. (Type / Format must match the format in the shader)
         * @param parameters Optional user-supplied shader parameters.
         * @param storageBuffers Optional user-supplied storage buffer bindings.
         * @param textureArrays Optional user-supplied texture bindings.
         */
        virtual void execute(rg::GraphBuilder &graph,
                             const RenderPipelineShader &shader,
                             const Recti &viewport,
                             std::vector<Attachment> attachments,
                             std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                             std::unordered_map<std::string, BufferBinding> storageBuffers,
                             std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays);
    };
}

#endif //XENGINE_RENDERPIPELINE_HPP
