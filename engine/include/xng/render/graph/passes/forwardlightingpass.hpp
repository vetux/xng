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

#ifndef XENGINE_FORWARDLIGHTINGPASS_HPP
#define XENGINE_FORWARDLIGHTINGPASS_HPP

#include "xng/render/graph/framegraphpass.hpp"
#include "xng/render/graph/framegraphtextureatlas.hpp"

#include "xng/render/textureatlas.hpp"

namespace xng {
    /**
     * The forward shading model implementation used for transparent objects.
     *
     * Writes SLOT_FORWARD_COLOR and SLOT_FORWARD_DEPTH.
     *
     * Reads SLOT_DEFERRED_DEPTH
     */
    class XENGINE_EXPORT ForwardLightingPass : public FrameGraphPass {
    public:
        void setup(FrameGraphBuilder &builder) override;

        void execute(FrameGraphPassResources &resources,
                     const std::vector<std::reference_wrapper<CommandQueue>> &renderQueues,
                     const std::vector<std::reference_wrapper<CommandQueue>> &computeQueues,
                     const std::vector<std::reference_wrapper<CommandQueue>> &transferQueues) override;

        std::type_index getTypeIndex() const override;

    private:
        struct MeshDrawData {
            Primitive primitive = TRIANGLES;
            DrawCall drawCall{};
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

        FrameGraphResource forwardColorRes;
        FrameGraphResource forwardDepthRes;

        FrameGraphResource deferredDepthRes;

        FrameGraphResource targetRes;
        FrameGraphResource pipelineRes;
        FrameGraphResource passRes;
        FrameGraphResource shaderBufferRes;

        FrameGraphResource pointLightsBufferRes;
        FrameGraphResource spotLightsBufferRes;
        FrameGraphResource directionalLightsBufferRes;

        FrameGraphResource vertexBufferRes;
        FrameGraphResource indexBufferRes;
        FrameGraphResource vertexArrayObjectRes;

        FrameGraphResource staleVertexBuffer;
        FrameGraphResource staleIndexBuffer;

        FrameGraphResource commandBuffer;

        FrameGraphTextureAtlas atlas;

        Camera camera;
        Transform cameraTransform;

        Vec2i renderSize;

        std::vector<Scene::Object> objects;

        std::vector<PointLight> pointLights;
        std::vector<SpotLight> spotLights;
        std::vector<DirectionalLight> directionalLights;

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

        std::set<Uri> usedTextures;
        std::set<Uri> usedMeshes;

        bool bindVao = true;

        size_t drawCycles;
    };
}
#endif //XENGINE_FORWARDLIGHTINGPASS_HPP
