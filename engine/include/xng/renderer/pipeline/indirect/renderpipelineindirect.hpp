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

#include <utility>

#include "xng/renderer/pipeline/renderpipeline.hpp"
#include "xng/renderer/pipeline/indirect/rendershadercompilerindirect.hpp"

#include "xng/renderer/stream/bufferstreamer.hpp"
#include "xng/shaderscript/objectstd140.hpp"

namespace xng {
    class RenderPipelineIndirect final : public RenderPipeline {
    public:
        RenderPipelineIndirect(rg::Heap &heap,
                               ChunkStreamer &chunkStreamer,
                               MaterialLayout _layout)
            : heap(heap),
              layout(std::move(_layout)),
              materialLayout(getMaterialLayout(layout)),
              materialStream(heap,
                             chunkStreamer,
                             materialLayout.getTotalSize()) {
        }

        ~RenderPipelineIndirect() override;

        const MaterialLayout &getMaterialLayout() override;

        RenderShaderCompiler &getCompiler() override;

        std::shared_ptr<RenderPipelineTransform> createTransform() override;

        std::shared_ptr<RenderPipelineMaterial> createMaterial() override;

        DrawID addDrawCall(std::shared_ptr<RenderPipelineTransform> transform,
                           std::shared_ptr<RenderPipelineMaterial> material,
                           const std::vector<RenderObjectHandle<RenderMesh> > &mesh,
                           int sortPriority) override;

        void removeDrawCall(DrawID id) override;

        void setCamera(const Vec3f &position, const Mat4f &view, const Mat4f &projection) override;

        void setEnableDistanceSort(bool enable) override;

        void prepare(rg::GraphBuilder &graph) override;

        void execute(rg::GraphBuilder &graph,
                     const RenderShader &shader,
                     const Recti &viewport,
                     std::vector<Attachment> attachments,
                     std::unordered_map<std::string, rg::ShaderPrimitive> parameters,
                     std::unordered_map<std::string, BufferBinding> storageBuffers,
                     std::unordered_map<std::string, std::vector<rg::TextureBinding> > textureArrays) override;

    private:
        class RenderPipelineTransformIndirect final : public RenderPipelineTransform {
        public:
            ~RenderPipelineTransformIndirect() override;

            void setTransform(const Transform &t) override;
        };

        class RenderPipelineMaterialIndirect final : public RenderPipelineMaterial {
        public:
            explicit RenderPipelineMaterialIndirect(const LayoutStd140 &layout)
                : object(layout) {
            }

            ~RenderPipelineMaterialIndirect() override = default;

            void setProperty(PropertyID attribute, rg::ShaderPrimitive value) override;

            void setTexture(TextureID texture, RenderObjectHandle<RenderTexture> value) override;

            const std::unordered_map<PropertyID, rg::ShaderPrimitiveType> &getAttributes() override;

            const std::unordered_set<TextureID> &getTextures() override;

        private:
            ObjectStd140 object;
        };

        struct DrawLocation {
            int sortPriority;
            BufferStreamer<RenderShaderCompilerIndirect::ShaderDrawCall::CPU>::Slot slot;
        };

        struct BufferAccessRange {
            size_t offset;
            size_t size;
        };

        struct DrawList {
            BufferStreamer<RenderShaderCompilerIndirect::ShaderDrawCall::CPU> drawCallBuffer;

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

        static LayoutStd140 getMaterialLayout(const MaterialLayout &layout) {
            LayoutStd140 ret("ShaderMaterial");
            for (auto &pair: layout.properties) {
                ret.add("attr" + std::to_string(pair.first), pair.second);
            }
            for (auto &texture: layout.textures) {
                RenderShaderCompilerIndirect::ShaderTexture::injectLayout(ret,
                                                                          RenderShaderCompilerIndirect::materialTexturePrefix
                                                                          + std::to_string(texture));
            }
            return ret;
        }

        rg::Heap &heap;

        const MaterialLayout layout;

        LayoutStd140 materialLayout;
        GenericBufferStreamer materialStream;

        // Per sortPriority draw lists which are drawn in order and each draw list is additionally sorted in prepass based on camera distance.
        std::map<int, DrawList> drawLists{};

        std::unordered_map<DrawID, DrawLocation> drawLocations{};
    };
}

#endif //XENGINE_RENDERPIPELINEINDIRECT_HPP
