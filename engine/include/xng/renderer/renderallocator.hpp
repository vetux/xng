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

#include "xng/renderer/objects/renderfont.hpp"
#include "xng/renderer/objects/paint/renderpainttext.hpp"
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
    class XENGINE_EXPORT RenderAllocator {
    public:
        struct Buffers {
            rg::HeapResource<rg::Buffer> meshBuffer;

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

            rg::HeapResource<rg::Texture> atlasTexture;
            rg::HeapResource<rg::Buffer> tileMapBuffer;
            rg::HeapResource<rg::Buffer> tileMapOffsetsBuffer;
            rg::HeapResource<rg::Buffer> residencyMapBuffer;
            rg::HeapResource<rg::Buffer> residencyMapOffsetsBuffer;
            rg::HeapResource<rg::Buffer> readbackBuffer;

            unsigned int atlasSize;
            unsigned int tileSize;
            unsigned int tileBorder;
            float maxAnisotropy;
        };

        /**
         * The streamingBudget may be exceeded when flushing objects.
         *
         * @param runtime
         * @param streamingBudget The total streaming budget to saturate in bytes. Should be multiple of 1KB.
         */
        explicit RenderAllocator(rg::Runtime &runtime, size_t streamingBudget);

        RenderObjectHandle<RenderTexture> createTexture(const std::shared_ptr<TileLoader> &tileLoader);

        RenderObjectHandle<RenderTexture> createTexture(const ImageRGBA &image, WrappingMethod wrapping);

        RenderObjectHandle<RenderMaterial> createMaterial(const ColorRGBA &albedo,
                                                          float metallic,
                                                          float roughness,
                                                          float ambientOcclusion,
                                                          float normalIntensity,
                                                          bool flipNormal,
                                                          RenderObjectHandle<RenderTexture> albedoTexture,
                                                          SamplingProperties albedoProperties,
                                                          RenderObjectHandle<RenderTexture> metallicTexture,
                                                          SamplingProperties metallicProperties,
                                                          RenderObjectHandle<RenderTexture> roughnessTexture,
                                                          SamplingProperties roughnessProperties,
                                                          RenderObjectHandle<RenderTexture> ambientOcclusionTexture,
                                                          SamplingProperties ambientOcclusionProperties,
                                                          RenderObjectHandle<RenderTexture> normalTexture,
                                                          SamplingProperties normalProperties);

        RenderObjectHandle<RenderSkeleton> createSkeleton(const std::vector<std::string> &boneNames);

        RenderObjectHandle<RenderMesh> createMesh(const Mesh &mesh, RenderObjectHandle<RenderSkeleton> skeleton);

        RenderObjectHandle<RenderModel> createModel(std::vector<RenderObjectHandle<RenderMesh> > meshes,
                                                    RenderObjectHandle<RenderMaterial> material,
                                                    RenderPath renderPath,
                                                    ShadingModel shadingModel,
                                                    bool receiveShadows,
                                                    bool castShadows);

        RenderObjectHandle<RenderPointLight> createPointLight();

        RenderObjectHandle<RenderSpotLight> createSpotLight();

        RenderObjectHandle<RenderDirectionalLight> createDirectionalLight();

        RenderObjectHandle<RenderFont> createFont(std::vector<std::unique_ptr<FontRenderer> > fonts,
                                                  const Vec2i &pixelSize);

        RenderObjectHandle<RenderPaintText> createPaintText(const RenderObjectHandle<RenderFont> &font,
                                                            const std::u32string &text,
                                                            const TextLayoutParameters &layoutParameters,
                                                            const ColorRGBA &color,
                                                            const SamplingProperties &sampling_properties);

        RenderObjectHandle<RenderCanvas> createCanvas();

        void destroy(const RenderObject &object);

        [[nodiscard]] Buffers commit(rg::GraphBuilder &graph);

        const VirtualTextureStreamer &getTextureStreamer() const {
            return textureStream;
        }

        const MeshStreamer &getMeshStreamer() const {
            return meshStream;
        }

    private:
        RenderObject::Id allocateId() {
            if (freeIds.empty()) {
                return nextId++;
            }
            const auto ret = freeIds.back();
            freeIds.pop_back();
            return ret;
        }

        void deallocateId(const RenderObject::Id id) {
            freeIds.push_back(id);
        }

        RenderObject::Id nextId = 0;
        std::vector<RenderObject::Id> freeIds;

        rg::Heap &heap;

        ChunkStreamer chunkStreamer;

        BufferStreamer<ShaderMesh::CPU> shaderMeshStream;

        BufferStreamer<ShaderTransform::CPU> transformStream;
        BufferStreamer<ShaderMaterial::CPU> materialStream;

        SkeletonStreamer skeletonStream;

        BufferStreamer<ShaderPointLight::CPU> pointLightStream;
        BufferStreamer<ShaderSpotLight::CPU> spotLightStream;
        BufferStreamer<ShaderDirectionalLight::CPU> directionalLightStream;

        MeshStreamer meshStream;
        VirtualTextureStreamer textureStream;
    };
}

#endif //XENGINE_RENDERALLOCATOR_HPP
