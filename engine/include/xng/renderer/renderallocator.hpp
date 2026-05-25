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

#ifndef XENGINE_RENDERALLOCATOR_HPP
#define XENGINE_RENDERALLOCATOR_HPP

#include "xng/assets/assetscene.hpp"

#include "xng/renderer/camera.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"
#include "xng/renderer/objects/rendermaterial.hpp"
#include "xng/renderer/objects/rendermesh.hpp"
#include "xng/renderer/objects/rendermodel.hpp"
#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderskeleton.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"
#include "xng/renderer/objects/rendertexture.hpp"
#include "xng/renderer/objects/renderpaint.hpp"
#include "xng/renderer/objects/rendercanvas.hpp"

namespace xng {
    class RenderAllocator {
    public:
        struct Buffers {
            rg::HeapResource<rg::Buffer> transformBuffer;
            rg::HeapResource<rg::Buffer> materialBuffer;

            rg::HeapResource<rg::Buffer> boneBuffer;

            rg::HeapResource<rg::Buffer> pointLightBuffer;
            rg::HeapResource<rg::Buffer> spotLightBuffer;
            rg::HeapResource<rg::Buffer> directionalLightBuffer;

            std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > vertexBuffers;
            rg::HeapResource<rg::Buffer> indexBuffer;

            rg::HeapResource<rg::Buffer> skinnedBindPosBuffer;
            rg::HeapResource<rg::Buffer> skinnedBoneIndicesBuffer;
            rg::HeapResource<rg::Buffer> skinnedBoneWeightsBuffer;

            std::unordered_map<TextureResolution, rg::HeapResource<rg::Texture> > textures;
        };

        /**
         * The streamingBudget may be exceeded when flushing objects.
         *
         * @param heap
         * @param streamingBudget The total streaming budget to saturate in bytes. Should be multiple of 1KB.
         */
        explicit RenderAllocator(rg::Heap &heap, size_t streamingBudget);

        RenderObjectHandle<RenderTexture> createTexture(const Vec2i &resolution);

        RenderObjectHandle<RenderMaterial> createMaterial(const ColorRGBA &albedo,
                                                          float metallic,
                                                          float roughness,
                                                          float ambientOcclusion,
                                                          const Vec4f &normalIntensity,
                                                          RenderObjectHandle<RenderTexture> albedoTexture,
                                                          RenderObjectHandle<RenderTexture> metallicTexture,
                                                          RenderObjectHandle<RenderTexture> roughnessTexture,
                                                          RenderObjectHandle<RenderTexture> ambientOcclusionTexture,
                                                          RenderObjectHandle<RenderTexture> normalTexture);

        RenderObjectHandle<RenderSkeleton> createSkeleton(const std::vector<std::string> &boneNames);

        RenderObjectHandle<RenderMesh> createMesh(const Mesh &mesh, RenderObjectHandle<RenderSkeleton> skeleton);

        RenderObjectHandle<RenderModel> createModel(std::vector<RenderObjectHandle<RenderMesh> > meshes,
                                                    RenderObjectHandle<RenderMaterial> material,
                                                    ShadingModel shadingModel,
                                                    bool receiveShadows,
                                                    bool castShadows);

        RenderObjectHandle<RenderPointLight> createPointLight();

        RenderObjectHandle<RenderSpotLight> createSpotLight();

        RenderObjectHandle<RenderDirectionalLight> createDirectionalLight();

        RenderObjectHandle<RenderPaint> createPaint();

        RenderObjectHandle<RenderCanvas> createCanvas();

        [[nodiscard]] Buffers commit(rg::GraphBuilder &graph);

    private:
        rg::Heap &heap;

        ChunkStreamer chunkStreamer;

        BufferStreamer<ShaderTransform::CPU> transformStream;
        BufferStreamer<ShaderMaterial::CPU> materialStream;

        SkeletonStreamer skeletonStream;

        BufferStreamer<ShaderPointLight::CPU> pointLightStream;
        BufferStreamer<ShaderSpotLight::CPU> spotLightStream;
        BufferStreamer<ShaderDirectionalLight::CPU> directionalLightStream;

        MeshStreamer meshStream;
        TextureStreamer textureStream;
    };
}

#endif //XENGINE_RENDERALLOCATOR_HPP
