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

#ifndef XENGINE_RENDERSCENE_HPP
#define XENGINE_RENDERSCENE_HPP

#include "xng/rendergraph/heap.hpp"

#include "xng/renderer/shadingmodel.hpp"
#include "xng/renderer/vertexattribute.hpp"
#include "xng/renderer/renderpath.hpp"
#include "xng/renderer/objects/rendermodel.hpp"
#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/rendercanvas.hpp"

#include "xng/shaderscript/indirectbuffers.hpp"

namespace xng {
    /**
     * The absolute (Packed) data to be drawn via indirect draw.
     *
     * Any kind of virtual geometry / culling / packing logic would sit in front of this.
     *
     * Passes only have to be rerecorded when any members of this struct are changed but not if the actual contents of the resources on the gpu change.
     *
     * Heap / Streaming / Residency concepts are hidden from consumers of the render scene.
     */
    struct RenderScene {
        struct BufferAccessRange {
            size_t offset;
            size_t size;
        };

        struct Batch {
            /**
             * The maximum number of commands in the indirect buffer.
             */
            size_t batchSize;

            /**
             * The stride between commands in the indirect buffer.
             */
            size_t stride;

            /**
             * The indirect buffer containing up to batchSize commands.
             *
             * The size of the indirect buffer is batchSize * sizeof(ShaderDrawIndirectIndexed)
             */
            rg::Resource<rg::Buffer> indirectBuffer;
            size_t indirectBufferOffset;

            /**
             * The count buffer containing the number of commands in the indirect buffer
             */
            rg::Resource<rg::Buffer> indirectCountBuffer;
            size_t indirectCountBufferOffset;

            /**
             * Which render path to use for this batch.
             * The scene prepass handles batch / draw ordering.
             */
            RenderPath renderPath;

            /**
             * The shading model to use for this batch.
             */
            ShadingModel shadingModel;

            /**
             * The set of byte ranges / texture layers in the buffers that may be read by this draw batch.
             */
            std::vector<BufferAccessRange> drawBufferAccesses;
            std::vector<BufferAccessRange> transformBufferAccesses;
            std::vector<BufferAccessRange> materialBufferAccesses;

            std::unordered_map<VertexAttribute, std::vector<BufferAccessRange> > vertexBufferAccesses;
            std::vector<BufferAccessRange> indexBufferAccesses;

            std::unordered_map<TextureResolution, std::vector<size_t> > textureAccesses;

            Batch(const size_t batchSize,
                  const size_t stride,
                  const rg::Resource<rg::Buffer> &indirectBuffer,
                  const size_t indirectBufferOffset,
                  const rg::Resource<rg::Buffer> &indirectCountBuffer,
                  const size_t indirectCountBufferOffset,
                  const RenderPath renderPath,
                  const ShadingModel shadingModel,
                  std::vector<BufferAccessRange> drawBufferAccesses,
                  std::vector<BufferAccessRange> transformBufferAccesses,
                  std::vector<BufferAccessRange> materialBufferAccesses,
                  std::unordered_map<VertexAttribute, std::vector<BufferAccessRange> > vertexBufferAccesses,
                  std::vector<BufferAccessRange> indexBufferAccesses,
                  std::unordered_map<TextureResolution, std::vector<size_t> > textureAccesses)
                : batchSize(batchSize),
                  stride(stride),
                  indirectBuffer(indirectBuffer),
                  indirectBufferOffset(indirectBufferOffset),
                  indirectCountBuffer(indirectCountBuffer),
                  indirectCountBufferOffset(indirectCountBufferOffset),
                  renderPath(renderPath),
                  shadingModel(shadingModel),
                  drawBufferAccesses(std::move(drawBufferAccesses)),
                  transformBufferAccesses(std::move(transformBufferAccesses)),
                  materialBufferAccesses(std::move(materialBufferAccesses)),
                  vertexBufferAccesses(std::move(vertexBufferAccesses)),
                  indexBufferAccesses(std::move(indexBufferAccesses)),
                  textureAccesses(std::move(textureAccesses)) {
            }
        };

        /**
         * The render batches for drawing the models.
         */
        std::vector<Batch> drawList;

        /**
         * The draw buffer indexed by the draw batches via GetBaseInstance + GetDrawID + GetInstanceID
         */
        rg::Resource<rg::Buffer> drawBuffer;

        /**
         * The buffers indexed via the indices in the model buffer
         */
        rg::Resource<rg::Buffer> transformBuffer;
        rg::Resource<rg::Buffer> materialBuffer;

        /**
         * The vertex / index buffers containing all geometry.
         * The vertex buffer positions are skinned positions.
         */
        std::unordered_map<VertexAttribute, rg::Resource<rg::Buffer> > vertexBuffers;
        rg::Resource<rg::Buffer> indexBuffer;

        /**
         * The texture arrays containing all textures.
         */
        std::unordered_map<TextureResolution, rg::Resource<rg::Texture> > textures;

        /**
         * The light buffers.
         */
        rg::Resource<rg::Buffer> pointLightBuffer;
        rg::Resource<rg::Buffer> spotLightBuffer;
        rg::Resource<rg::Buffer> directionalLightBuffer;

        /**
         * The camera buffer.
         */
        rg::Resource<rg::Buffer> cameraBuffer;

        /**
         * The config buffer.
         */
        rg::Resource<rg::Buffer> configBuffer;

        //TODO: Design canvas rendering strategy

        /**
         * The paint buffer indexed by the canvas model buffer.
         */
        rg::Resource<rg::Buffer> paintBuffer;

        std::vector<RenderObjectHandle<RenderCanvas> > canvases;
    };
}

#endif //XENGINE_RENDERSCENE_HPP
