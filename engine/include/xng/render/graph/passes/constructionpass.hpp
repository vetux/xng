/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_CONSTRUCTIONPASS_HPP
#define XENGINE_CONSTRUCTIONPASS_HPP

#include <unordered_set>

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/resource/uri.hpp"
#include "xng/render/scene.hpp"

#include "xng/render/graph/framegraphtextureatlas.hpp"

namespace xng {
    /**
     * The ConstructionPass creates and assigns the geometry buffer slot textures and creates and assigns the SLOT_DEFERRED_* and SLOT_FORWARD_* slot textures.
     */
    class XENGINE_EXPORT ConstructionPass : public FrameGraphPass {
    public:
        ConstructionPass();

        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources) override;

        std::type_index getTypeIndex() const override;

    private:
        struct MeshDrawData {
            Primitive primitive = TRIANGLES;
            RenderPass::DrawCall drawCall{};
            size_t baseVertex = 0;
        };

        TextureAtlasHandle getTexture(const ResourceHandle <Texture> &texture,
                                      std::map<TextureAtlasResolution, std::reference_wrapper<TextureArrayBuffer>> &atlasBuffers);

        MeshDrawData getMesh(const ResourceHandle<Mesh> &mesh);

        void prepareMeshAllocation(const ResourceHandle<Mesh> &mesh);

        void allocateMeshes(VertexBuffer &vertexBuffer, IndexBuffer &indexBuffer);

        void deallocateMesh(const ResourceHandle<Mesh> &mesh);

        void deallocateTexture(const ResourceHandle<Texture> &texture);

        /**
         * @param size number of bytes to allocate
         * @return The offset in bytes into the index buffer
         */
        size_t allocateVertexData(size_t size);

        void deallocateVertexData(size_t offset);

        /**
         * @param size number of bytes to allocate
         * @return The offset in bytes into the index buffer
         */
        size_t allocateIndexData(size_t size);

        void deallocateIndexData(size_t offset);

        void mergeFreeVertexBufferRanges();

        void mergeFreeIndexBufferRanges();

        FrameGraphResource renderTargetRes;
        FrameGraphResource renderPipelineRes;
        FrameGraphResource renderPassRes;
        FrameGraphResource shaderBufferRes;

        FrameGraphResource clearTargetRes;
        FrameGraphResource clearPassRes;

        FrameGraphResource vertexBufferRes;
        FrameGraphResource indexBufferRes;
        FrameGraphResource vertexArrayObjectRes;

        FrameGraphResource staleVertexBuffer;
        FrameGraphResource staleIndexBuffer;

        FrameGraphTextureAtlas atlas;

        FrameGraphResource gBufferPosition;
        FrameGraphResource gBufferNormal;
        FrameGraphResource gBufferTangent;
        FrameGraphResource gBufferRoughnessMetallicAmbientOcclusion;
        FrameGraphResource gBufferAlbedo;
        FrameGraphResource gBufferAmbient;
        FrameGraphResource gBufferSpecular;
        FrameGraphResource gBufferModelObject;
        FrameGraphResource gBufferDepth;

        FrameGraphResource screenColor;
        FrameGraphResource screenDepth;

        FrameGraphResource deferredColor;
        FrameGraphResource deferredDepth;

        FrameGraphResource forwardColor;
        FrameGraphResource forwardDepth;

        Camera camera;
        Transform cameraTransform;

        std::vector<Scene::Object> objects;

        std::map<Uri, MeshDrawData> meshAllocations;
        std::map<Uri, MeshDrawData> pendingMeshAllocations;
        std::map<Uri, ResourceHandle<Mesh>> pendingMeshHandles;

        size_t currentVertexBufferSize{};
        size_t currentIndexBufferSize{};
        size_t requestedVertexBufferSize{};
        size_t requestedIndexBufferSize{};

        std::map<size_t, size_t> freeVertexBufferRanges; // start and size of free ranges of vertices with layout vertexLayout in the vertex buffer
        std::map<size_t, size_t> freeIndexBufferRanges; // start and size of free ranges of bytes in the index buffer

        std::map<size_t, size_t> allocatedVertexRanges;
        std::map<size_t, size_t> allocatedIndexRanges;

        std::map<Uri, TextureAtlasHandle> textures;

        Vec2i renderSize;

        std::set<Uri> usedTextures;
        std::set<Uri> usedMeshes;

        bool bindVao = true;

        size_t drawCycles;
    };
}
#endif //XENGINE_CONSTRUCTIONPASS_HPP
