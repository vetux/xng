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
#include "xng/renderer/pipeline/indirect/renderpipelinecompilerindirect.hpp"

#include "xng/renderer/stream/bufferstreamer.hpp"
#include "xng/shaderscript/objectstd140.hpp"

namespace xng {
    class XENGINE_EXPORT RenderPipelineIndirect final : public RenderPipeline {
    public:
        static rg::Shader getPrePassShader();

        RenderPipelineIndirect(rg::Runtime &runtime,
                               ChunkStreamer &chunkStreamer,
                               MeshStreamer &meshStreamer,
                               VirtualTextureStreamer &virtualTextureStreamer,
                               MaterialLayout _layout,
                               rg::Shader prePassShader)
            : heap(runtime.getResourceHeap()),
              chunkStreamer(chunkStreamer),
              meshStreamer(meshStreamer),
              virtualTextureStreamer(virtualTextureStreamer),
              layout(std::move(_layout)),
              materialLayout(getMaterialLayout(layout)),
              cameraBuffer(runtime.getResourceHeap(),
                           chunkStreamer,
                           rg::Buffer::CAPABILITY_STORAGE,
                           sizeof(RenderPipelineCompilerIndirect::ShaderCamera::CPU)),
              materialStreamer(heap,
                               chunkStreamer,
                               materialLayout.getTotalSize()),
              transformStreamer(heap, chunkStreamer),
              compiler(runtime.getPipelineCache(),
                       layout,
                       materialLayout),
              prePassPipeline(compilePrePassPipeline(runtime, std::move(prePassShader))) {
        }

        ~RenderPipelineIndirect() override;

        const MaterialLayout &getMaterialLayout() override {
            return layout;
        }

        const RenderPipelineCompiler &getCompiler() const override {
            return compiler;
        }

        std::shared_ptr<RenderPipelineTransform> createTransform() override;

        std::shared_ptr<RenderPipelineMaterial> createMaterial() override;

        DrawID addDrawCall(std::shared_ptr<RenderPipelineTransform> transform,
                           std::shared_ptr<RenderPipelineMaterial> material,
                           const std::vector<RenderObjectHandle<RenderMesh> > &mesh,
                           int sortPriority) override;

        DrawID addDrawCall(std::shared_ptr<RenderPipelineTransform> transform,
                           const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                           int sortPriority) override;

        void removeDrawCall(DrawID id) override;

        void setCamera(const Vec3f &position, const Mat4f &view, const Mat4f &projection) override;

        void setEnableDistanceSort(bool enable) override;

        void setEnableDrawCulling(bool enable) override;

        void commit(RenderQueue &queue) override;

        void prepare(RenderQueue &queue) override;

        void execute(rg::GraphBuilder &graph,
                     const std::string &passName,
                     const RenderPipelineShader &shader,
                     const Recti &viewport,
                     const std::vector<Attachment> &colorAttachments,
                     std::optional<rg::Attachment> depthStencilAttachment,
                     std::optional<rg::Attachment> depthAttachment,
                     std::optional<rg::Attachment> stencilAttachment,
                     const std::unordered_map<std::string, rg::ShaderPrimitive> &parameters,
                     const std::unordered_map<std::string, BufferBinding> &storageBuffers,
                     const std::unordered_map<std::string, std::vector<rg::TextureBinding> > &textureArrays)
        const override;

    private:
        static constexpr int prePassLocalSize = 256;

        class RenderPipelineTransformIndirect final : public RenderPipelineTransform {
        public:
            RenderPipelineTransformIndirect(BufferStreamer<Mat4f> &buffer,
                                            const BufferStreamer<Mat4f>::Slot slot)
                : buffer(buffer), slot(slot) {
            }

            ~RenderPipelineTransformIndirect() override {
                buffer.destroy(slot);
            }

            void setTransform(const Transform &t) override {
                buffer.upload(slot, t.model());
            }

            void setTransform(const Mat4f &model) override {
                buffer.upload(slot, model);
            }

            [[nodiscard]] BufferStreamer<Mat4f>::Slot getSlot() const {
                return slot;
            }

            bool isUploadComplete() override {
                return buffer.isUploadComplete(slot);
            }

            void flush() override {
                return buffer.flush(slot);
            }

        private:
            BufferStreamer<Mat4f> &buffer;
            BufferStreamer<Mat4f>::Slot slot;
        };

        class RenderPipelineMaterialIndirect final : public RenderPipelineMaterial {
        public:
            explicit RenderPipelineMaterialIndirect(GenericBufferStreamer &buffer,
                                                    const GenericBufferStreamer::Slot slot,
                                                    LayoutStd140 layout)
                : buffer(buffer),
                  slot(slot),
                  layout(std::move(layout)) {
            }

            ~RenderPipelineMaterialIndirect() override {
                buffer.destroy(slot);
            }

            void update(const std::unordered_map<PropertyID, rg::ShaderPrimitive> &properties,
                        const std::unordered_map<TextureID, TextureSampler> &textures) override {
                ObjectStd140 obj(layout);
                for (auto &pair: properties) {
                    obj.set(RenderPipelineCompilerIndirect::materialPropertyPrefix + std::to_string(pair.first),
                            pair.second);
                }
                const auto &prefix = RenderPipelineCompilerIndirect::materialTexturePrefix;
                for (auto &pair: textures) {
                    const auto name = prefix + std::to_string(pair.first);
                    const auto &tex = pair.second.texture.get();
                    const auto &sp = pair.second.samplingProperties;
                    RenderPipelineCompilerIndirect::ShaderTexture::setTextureBacking(name,
                        obj,
                        tex.getBacking());
                    RenderPipelineCompilerIndirect::ShaderTexture::setTextureID(name,
                        obj,
                        tex.getHandle());
                    RenderPipelineCompilerIndirect::ShaderTexture::setTextureSize(name,
                        obj,
                        tex.getSize().convert<int>());
                    RenderPipelineCompilerIndirect::ShaderTexture::setMaxMip(name,
                                                                             obj,
                                                                             tex.getMaxMip());
                    RenderPipelineCompilerIndirect::ShaderTexture::setMinFilter(name,
                        obj,
                        sp.minFilter);
                    RenderPipelineCompilerIndirect::ShaderTexture::setMagFilter(name,
                        obj,
                        sp.magFilter);
                    RenderPipelineCompilerIndirect::ShaderTexture::setMipFilter(name,
                        obj,
                        sp.mipFilter);
                    RenderPipelineCompilerIndirect::ShaderTexture::setWrap(name,
                                                                           obj,
                                                                           sp.wrapping);
                    RenderPipelineCompilerIndirect::ShaderTexture::setSrcRect(name,
                                                                              obj,
                                                                              Vec4f(sp.srcRect.position.x,
                                                                                  sp.srcRect.position.y,
                                                                                  sp.srcRect.dimensions.x,
                                                                                  sp.srcRect.dimensions.y));
                }
                buffer.upload(slot, obj.getData().data(), obj.getData().size());

                textureSamplers = textures;
            }

            bool isUploadComplete() override {
                return buffer.isUploadComplete(slot);
            }

            void flush() override {
                buffer.flush(slot);
            }

            GenericBufferStreamer::Slot getSlot() const {
                return slot;
            }

        private:
            GenericBufferStreamer &buffer;
            GenericBufferStreamer::Slot slot;

            LayoutStd140 layout;

            std::unordered_map<TextureID, TextureSampler> textureSamplers;
        };

        ShaderStruct(ShaderDrawCall,
                     int, baseVertex,
                     unsigned int, indexOffset, // The first index
                     unsigned int, indexCount,
                     unsigned int, transformIndex,
                     unsigned int, materialIndex,
                     unsigned int, padding0,
                     unsigned int, padding1,
                     unsigned int, padding2)

        struct BufferAccessRange {
            size_t offset;
            size_t size;

            bool operator==(const BufferAccessRange &other) const {
                return offset == other.offset && size == other.size;
            }
        };

        struct BufferAccessRangeHasher {
            size_t operator()(const BufferAccessRange &p) const noexcept {
                size_t ret;
                hash_combine(ret, p.offset);
                hash_combine(ret, p.size);
                return ret;
            }
        };

        struct DrawCall {
            int sortPriority{};

            std::shared_ptr<RenderPipelineTransform> transform;
            std::shared_ptr<RenderPipelineMaterial> material;
            std::vector<RenderObjectHandle<RenderMesh> > meshes;

            std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> transformAccessRanges;
            std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> materialAccessRanges;
            std::unordered_map<VertexAttribute, std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> >
            vertexBufferAccessRanges;
            std::unordered_set<BufferAccessRange, BufferAccessRangeHasher> indexBufferAccessRanges;

            std::vector<ShaderDrawCall::CPU> drawCallData;

            DrawCall() = default;

            DrawCall(std::shared_ptr<RenderPipelineTransform> _transform,
                     std::shared_ptr<RenderPipelineMaterial> _material,
                     const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
                     const int sortPriority)
                : sortPriority(sortPriority),
                  transform(std::move(_transform)),
                  material(std::move(_material)),
                  meshes(meshes) {
            }

            bool isUploadComplete() {
                for (auto &mesh: meshes) {
                    if (!mesh.get().isUploadComplete()) {
                        return false;
                    }
                }
                return transform->isUploadComplete() && material->isUploadComplete();
            }

            void loadData() {
                assert(isUploadComplete());
                for (auto &mesh: meshes) {
                    const auto alloc = mesh.get().getAllocation();
                    ShaderDrawCall::CPU c;
                    c.baseVertex = alloc.baseVertex;
                    c.indexOffset = alloc.drawCall.offset / sizeof(unsigned int);
                    c.indexCount = alloc.drawCall.count;

                    const auto &dt = down_cast<RenderPipelineTransformIndirect &>(*transform);
                    c.transformIndex = dt.getSlot();

                    const auto &dm = down_cast<RenderPipelineMaterialIndirect &>(*material);
                    c.materialIndex = dm.getSlot();

                    drawCallData.emplace_back(c);

                    transformAccessRanges.insert({dt.getSlot() * sizeof(Mat4f), sizeof(Mat4f)});
                    materialAccessRanges.insert({dm.getSlot() * sizeof(Mat4f), sizeof(Mat4f)});
                    for (auto attr = ATTRIBUTE_BEGIN;
                         attr <= ATTRIBUTE_END;
                         attr = static_cast<VertexAttribute>(attr + 1)) {
                        vertexBufferAccessRanges[attr].insert({
                            alloc.baseVertex * getVertexAttributeSize(attr),
                            alloc.vertexCount * getVertexAttributeSize(attr)
                        });
                    }
                    indexBufferAccessRanges.insert({
                        alloc.drawCall.offset,
                        alloc.drawCall.count * sizeof(unsigned int)
                    });
                }
            }
        };

        // TODO: Shared indirect buffers
        // TODO: Shared draw call / mesh buffers
        // TODO: Batch based fixed size indirect buffers
        // TODO: Automatic Instancing
        struct DrawList {
            std::unordered_set<DrawID> drawCalls;

            StreamBuffer drawCallBuffer;
            StreamBuffer::Handle drawCallBufferHandle{};

            bool updateDrawCallBuffer = false;
            bool residentDrawCallBuffer = false;

            rg::HeapResource<rg::Buffer> indirectBuffer{};
            rg::HeapResource<rg::Buffer> indirectCountBuffer{};

            rg::HeapResource<rg::Buffer> drawMeshBuffer{};

            // By explicitly tracking all subregion accesses, the graphics queue can
            // overlap pending chunk -> buffer copies with rendering.
            // Comes at a cpu overhead but can be kept to a minimum.
            std::vector<BufferAccessRange> transformAccesses;
            std::vector<BufferAccessRange> materialAccesses;
            std::unordered_map<VertexAttribute, std::vector<BufferAccessRange> > vertexBufferAccesses;
            std::vector<BufferAccessRange> indexBufferAccesses;

            // For textures the problem is that hardware doesn't support tracking accesses at texture subresource level (Only mip / layer).
            // This means the graphics queue cannot overlap copies of tiles from chunk -> texture with
            // rendering unless streamed tiles are copied into a layer which is not in use. This is very hard to implement
            // so i think we just have to deal with the overhead by limiting the amount of tiles copied in a given frame.
            std::vector<size_t> virtualTextureAccesses;

            DrawList(rg::Heap &resourceHeap, ChunkStreamer &chunkStreamer);

            void commit(RenderQueue &queue,
                        rg::Heap &resourceHeap,
                        const std::unordered_map<DrawID, DrawCall> &callMap);

            size_t getDrawCallCount(const std::unordered_map<DrawID, DrawCall> &drawMap) const {
                size_t ret = 0;
                for (auto &id: drawCalls) {
                    ret += drawMap.at(id).drawCallData.size();
                }
                return ret;
            }
        };

        static LayoutStd140 getMaterialLayout(const MaterialLayout &layout) {
            LayoutStd140 ret("ShaderMaterial");
            for (auto &pair: layout.properties) {
                ret.add(RenderPipelineCompilerIndirect::materialPropertyPrefix + std::to_string(pair.first),
                        pair.second);
            }
            for (auto &texture: layout.textures) {
                RenderPipelineCompilerIndirect::ShaderTexture::injectLayout(ret,
                                                                            RenderPipelineCompilerIndirect::materialTexturePrefix
                                                                            + std::to_string(texture));
            }
            return ret;
        }

        static rg::PipelineCache::Handle compilePrePassPipeline(rg::Runtime &runtime, rg::Shader shader);

        void recordPrePass(RenderQueue &queue, const DrawList &drawList) const;

        DrawID allocateID() {
            if (freeIDs.empty()) {
                return nextID++;
            }
            const auto ret = freeIDs.back();
            freeIDs.pop_back();
            return ret;
        }

        void freeID(const DrawID id) {
            freeIDs.push_back(id);
        }

        DrawID nextID = 0;
        std::vector<DrawID> freeIDs;

        rg::Heap &heap;
        ChunkStreamer &chunkStreamer;
        MeshStreamer &meshStreamer;
        VirtualTextureStreamer &virtualTextureStreamer;

        const MaterialLayout layout;
        LayoutStd140 materialLayout;

        StreamBuffer cameraBuffer;
        StreamBuffer::Handle cameraBufferHandle{};
        bool cameraResident = false;

        GenericBufferStreamer materialStreamer;
        BufferStreamer<Mat4f> transformStreamer;

        // Per sortPriority draw lists which are drawn in order and each draw list is additionally sorted in prepass based on camera distance.
        std::map<int, DrawList> drawLists{};

        std::unordered_map<DrawID, DrawCall> drawCalls{};
        std::unordered_set<DrawID> pendingDrawCalls{};

        RenderPipelineCompilerIndirect compiler;

        rg::PipelineCache::Handle prePassPipeline;
    };
}

#endif //XENGINE_RENDERPIPELINEINDIRECT_HPP
