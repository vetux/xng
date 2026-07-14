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

#ifndef XENGINE_RENDERPIPELINEINDIRECT_HPP
#define XENGINE_RENDERPIPELINEINDIRECT_HPP

#include "xng/renderer/pipeline/renderpipeline.hpp"

#include "xng/renderer/pipeline/indirect/rendershadercompilerindirect.hpp"

namespace xng {
    class RenderPipelineIndirect final : public RenderPipeline {
    public:
        explicit RenderPipelineIndirect(rg::Heap &heap);

        ~RenderPipelineIndirect() override;

        DrawID addDrawCall(const RenderObjectHandle<RenderTransform> &transform,
                           const RenderObjectHandle<RenderMaterial> &material,
                           const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                           bool receiveShadows,
                           int sortPriority) override;

        void removeDrawCall(DrawID id) override;

        void setPointLights(const std::vector<RenderObjectHandle<RenderPointLight> > &lights) override;

        void setDirectionalLights(const std::vector<RenderObjectHandle<RenderDirectionalLight> > &lights) override;

        void setSpotLights(const std::vector<RenderObjectHandle<RenderSpotLight> > &lights) override;

        void setCamera(const Vec3f &position, const Mat4f &view, const Mat4f &projection) override;

        void setGamma(float gamma) override;

        void setEnableDistanceSort(bool enable) override;

        void prepare(rg::Graph &graph) override;

        void execute(rg::Graph &graph,
                     const RenderShader &shader,
                     std::vector<Attachment> attachments,
                     std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                     std::unordered_map<std::string, BufferBinding> storageBuffers,
                     std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) override;

    private:
        ShaderStruct(ShaderDrawCall,
                     unsigned int, transformIndex,
                     unsigned int, materialIndex,
                     Vec4i, receiveShadows)

        struct BufferAccessRange {
            size_t offset;
            size_t size;
        };

        struct DrawList {
            BufferStreamer<ShaderDrawCall> drawCallBuffer;

            rg::HeapResource<rg::Buffer> indirectBuffer;
            size_t indirectBufferOffset;

            rg::HeapResource<rg::Buffer> indirectCountBuffer;
            size_t indirectCountBufferOffset;

            std::vector<BufferAccessRange> transformAccessRanges;
            std::vector<BufferAccessRange> materialAccessRanges;

            std::unordered_map<VertexAttribute, std::vector<BufferAccessRange> > vertexBufferAccesses;
            std::vector<BufferAccessRange> indexBufferAccesses;

            std::vector<size_t> textureAccesses;
        };

        rg::Heap &heap;

        // Per sortPriority draw lists which are drawn in order and each draw list is additionally sorted in prepass based on camera distance.
        std::map<int, DrawList> drawLists;
    };
}

#endif //XENGINE_RENDERPIPELINEINDIRECT_HPP
